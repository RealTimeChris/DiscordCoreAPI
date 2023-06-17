<?php

/**
 * Automatic CI process for generating new vcpkg releases.
 * Based loosely on RealTimeChris's shell script version.
 * 
 * This updates the content of ./Vcpkg directory within the DiscordCoreAPI
 * repository on the main branch, which can then be diffed into
 * the microsoft/vcpkg main branch to build a PR for the new
 * release.
 * 
 * The procedure for this is:
 * 
 * 1) Generate various configuration files and put them into the
 *    systemwide vcpkg installation inside the CI container
 * 2) Attempt to build the package from the release tag,
 *    this will fail due to invalid SHA512 sum and return the
 *    correct SHA512 sum in the error output. Inability to get the
 *    SHA512 sum here will return nonzero from the script, failing
 *    the CI action.
 * 3) Capture the SHA512 from the error output, switch to main
 * 4) Copy the correct configuration into both the systemwide
 *    vcpkg install in the container, and into the vcpkg directory
 *    of main branch.
 * 5) Rerun the `vcpkg install` process again to verify it is working.
 *    A build failure here will return a nonzero return code from
 *    the script, failing the CI action.
 */

require __DIR__ . '/Vendor/autoload.php';
use DiscordCoreAPI\Packager\Vcpkg;

$vcpkg = new Vcpkg();

/* Check out source for latest tag */
if (!$vcpkg->checkoutRepository($vcpkg->getTag())) {
    exit(1);
}

/* First run with SHA512 of 0 to gather actual value and save it */
$sha512 = $vcpkg->firstBuild(
    $vcpkg->constructPortAndVersionFile()
);
if (!empty($sha512)) {
    /* Now check out main */
    if (!$vcpkg->checkoutRepository()) {
        exit(1);
    }

    /* Attempt second build with the valid SHA512 sum. Program exit
     * status is the exit status of `vcpkg install`
     */
    exit(
        $vcpkg->secondBuild(
            $vcpkg->constructPortAndVersionFile($sha512)
        )
    );
    
}

/* Error if no SHA sum could be generated */
exit(1);