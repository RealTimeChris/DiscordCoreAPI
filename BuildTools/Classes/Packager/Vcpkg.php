<?php

namespace DiscordCoreAPI\Packager;

use \RuntimeException;

const GREEN = "\033[32m";
const RED   = "\033[31m";
const WHITE = "\033[0m";

class Vcpkg
{
    /**
     * @var string Git Tag
     */
    private string $latestTag;

    /**
     * @var string Semver version
     */
    private string $version;

    /**
     * @var string Path to GIT
     */
    private string $git;

    /**
     * @var string Path to SUDO
     */
    private string $sudo;

    /**
     * @var bool True when we have done the first build to get the SHA512 sum
     */
    private bool $firstBuildComplete = false;

    /**
     * Constructor
     * 
     * Examines current diretory's git repository to get latest tag and version.
     */
    public function __construct()
    {
        global $argv;
        if (count($argv) < 2) {
            throw new RuntimeException(RED . "Missing github repository owner and access token\n" . WHITE);
        }
        echo GREEN . "Starting vcpkg updater...\n" . WHITE;
            
        /* Get the latest tag from the version of the repository checked out by default into the action */
        $this->latestTag = preg_replace("/\n/", "", shell_exec("git describe --tags `git rev-list --tags --max-count=1`"));
        $this->version = preg_replace('/^v/', '', $this->getTag());
        echo GREEN . "Latest tag: " . $this->getTag() . " version: " . $this->getVersion() . "\n" . WHITE;

	$this->git = trim(`which git`);
	$this->sudo = trim(`which sudo`);
    }

    /**
     * Get semver version
     *
     * @return string
     */
    public function getVersion(): string
    {
        return $this->version;
    }

    /**
     * Get the git tag we are building
     * 
     * @return string
     */
    public function getTag(): string
    {
        return $this->latestTag;
    }

    private function git(string $parameters, bool $sudo = false): void
    {
        system(($sudo ? $this->sudo . ' ' : '') . $this->git . ' ' . $parameters);
    }

    private function sudo(string $command): void
    {
        system($this->sudo . ' ' . $command);
    }

    /**
     * Check out a repository by tag or branch name to ~/discordcoreapi,
     * using the personal access token and userName passed in as command line parameters.
     * 
     * @param string $tag Tag to clone
     * @return bool false if the repository could not be cloned
     */
    function checkoutRepository(string $tag = ""): bool
    {
        global $argv;

        if (empty($tag)) {
            /* Empty tag means use the main branch */
            $tag = `{$this->git} config --get init.defaultBranch || echo main`;
        }
	$repositoryUrl = 'https://' . urlencode($argv[1]) . ':' . urlencode($argv[2]) . '@github.com/realtimechris/DiscordCoreAPI';

        echo GREEN . "Check out repository: $tag (user: ". $argv[1] . " branch: " . $tag . ")\n" . WHITE;

        chdir(getenv('HOME'));
        system('rm -rf ./discordcoreapi');
        $this->git('config --global user.email "40668522+RealTimeChris@users.noreply.github.com"');
        $this->git('config --global user.name "RealTimeChris"');
        $this->git('clone ' . escapeshellarg($repositoryUrl) . ' ./discordcoreapi --depth=1');
        
        /* This is noisy, silence it */
	$status = chdir(getenv("HOME") . '/discordcoreapi');
        $this->git('fetch -at 2>/dev/null');
        $this->git('checkout ' . escapeshellarg($tag) . ' 2>/dev/null');
	
	return $status;
    }

    /**
     * Create ./Vcpkg/ports/discordcoreapi/vcpkg.json and return the portfile contents to
     * build the branch that is cloned at ~/discordcoreapi
     * 
     * @param string $sha512 The SHA512 sum of the tagged download, or initially
     * zero, which means that the vcpkg install command should obtain it the
     * second time around.
     * @return string The portfile content
     */
    function constructPortAndVersionFile(string $sha512 = "0"): string
    {
        echo GREEN . "Construct portfile for " . $this->getVersion() . ", sha512: $sha512\n" . WHITE;
        chdir(getenv("HOME") . '/discordcoreapi');

        $portFileContent = 'if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO RealTimeChris/DiscordCoreAPI
    REF "v${VERSION}"
    SHA512 ' . $sha512 . '
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(NO_PREFIX_CORRECTION)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/License.md")
';
        // ./Vcpkg/ports/discordcoreapi/vcpkg.json
        $versionFileContent = '{
  "name": "discordcoreapi",
  "version": ' . json_encode($this->getVersion()) . ',
  "description": "A Discord bot library written in C++ using custom asynchronous coroutines.",
  "homepage": "https://discordcoreapi.com",
  "license": "MIT",
  "supports": "(windows & x64 & !xbox) | (linux & x64)",
  "dependencies": [
    "jsonifier",
    "libsodium",
    "openssl",
    "opus",
    {
      "name": "vcpkg-cmake",
      "host": true
    },
    {
      "name": "vcpkg-cmake-config",
      "host": true
    }
  ]
}';
        echo GREEN . "Writing portfile...\n" . WHITE;
        file_put_contents('./Vcpkg/ports/discordcoreapi/vcpkg.json', $versionFileContent);
        return $portFileContent;
    }

    /**
     * Attempt the first build of the vcpkg port. This will always fail, as it is given
     * an SHA512 sum of 0. When it fails the output contains the SHA512 sum, which is then
     * extracted from the error output using a regular expression, and saved for a second
     * attempt.
     * @param string $portFileContent Portfile content from constructPortAndVersionFile()
     * with an SHA512 sum of 0 passed in.
     * @return string SHA512 sum of build output
     */
    function firstBuild(string $portFileContent): string
    {
        echo GREEN . "Starting first build\n" . WHITE;

        chdir(getenv("HOME") . '/discordcoreapi');
        echo GREEN . "Create /usr/local/share/vcpkg/ports/discordcoreapi/\n" . WHITE;
        $this->sudo('mkdir -p /usr/local/share/vcpkg/ports/discordcoreapi/');
        echo GREEN . "Copy vcpkg.json to /usr/local/share/vcpkg/ports/discordcoreapi/vcpkg.json\n" . WHITE;
        $this->sudo('cp -v -R ./Vcpkg/ports/discordcoreapi/vcpkg.json /usr/local/share/vcpkg/ports/discordcoreapi/vcpkg.json');
        file_put_contents('/tmp/portfile', $portFileContent);
        $this->sudo('cp -v -R /tmp/portfile /usr/local/share/vcpkg/ports/discordcoreapi/portfile.cmake');
        unlink('/tmp/portfile');
        $buildResults = shell_exec($this->sudo . ' /usr/local/share/vcpkg/vcpkg install discordcoreapi:x64-linux');
        $matches = [];
        if (preg_match('/Actual hash:\s+([0-9a-fA-F]+)/', $buildResults, $matches)) {
            echo GREEN . "Obtained SHA512 for first build: " . $matches[1] . "\n" . WHITE;
            $this->firstBuildComplete = true;
            return $matches[1];
        }
        echo RED . "No SHA512 found during first build :(\n" . WHITE;
        return '';
    }

    /**
     * Second build using a valid SHA512 sum. This attempt should succeed, allowing us to push
     * the changed vcpkg portfiles into the main branch, where they can be used in a PR to
     * microsoft/vcpkg repository later.
     * 
     * @param string $portFileContent the contents of the portfile, containing a valid SHA512
     * sum from the first build attempt.
     * @return bool False if the build failed
     */
    function secondBuild(string $portFileContent): bool
    {

        if (!$this->firstBuildComplete) {
            throw new RuntimeException("No SHA512 sum is available, first build has not been run!");
        }

        echo GREEN . "Executing second build\n" . WHITE;
        echo GREEN . "Copy local port files to /usr/local/share...\n" . WHITE;
        chdir(getenv("HOME") . '/discordcoreapi');
        file_put_contents('./Vcpkg/ports/discordcoreapi/portfile.cmake', $portFileContent);
        $this->sudo('cp -v -R ./Vcpkg/ports/discordcoreapi/vcpkg.json /usr/local/share/vcpkg/ports/discordcoreapi/vcpkg.json');
        $this->sudo('cp -v -R ./Vcpkg/ports/discordcoreapi/portfile.cmake /usr/local/share/vcpkg/ports/discordcoreapi/portfile.cmake');
        $this->sudo('cp -v -R ./Vcpkg/ports/* /usr/local/share/vcpkg/ports/');

        echo GREEN . "vcpkg x-add-version...\n" . WHITE;
        chdir('/usr/local/share/vcpkg');
        $this->sudo('./vcpkg format-manifest ./ports/discordcoreapi/vcpkg.json');
        /* Note: We commit this in /usr/local, but we never push it (we can't) */
        $this->git('add .', true);
        $this->git('commit -m "VCPKG info update"', true);
        $this->sudo('/usr/local/share/vcpkg/vcpkg x-add-version discordcoreapi');

        echo GREEN . "Copy back port files from /usr/local/share...\n" . WHITE;
        chdir(getenv('HOME') . '/discordcoreapi');
        system('cp -v -R /usr/local/share/vcpkg/ports/discordcoreapi/vcpkg.json ./Vcpkg/ports/discordcoreapi/vcpkg.json');
        system('cp -v -R /usr/local/share/vcpkg/versions/d-/discordcoreapi.json ./Vcpkg/versions/d-/discordcoreapi.json');

        echo GREEN . "Commit and push changes to main branch\n" . WHITE;
        $this->git('add .');
        $this->git('commit -m "VCPKG info update [skip ci]"');
        $this->git('config pull.rebase false');
        $this->git('pull');
        $this->git('push origin main');

        echo GREEN . "vcpkg install...\n" . WHITE;
        $resultCode = 0;
        $output = [];
        exec($this->sudo . ' /usr/local/share/vcpkg/vcpkg install discordcoreapi:x64-linux', $output, $resultCode);

        if ($resultCode != 0) {
            echo RED . "There were build errors!\n\nBuild log:\n" . WHITE;
            readfile("/usr/local/share/vcpkg/buildtrees/discordcoreapi/install-x64-linux-dbg-out.log");
        }

	return $resultCode == 0;
    }
};
