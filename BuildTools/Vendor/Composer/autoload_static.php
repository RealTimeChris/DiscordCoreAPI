<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInit0e8415491642f27914717986db49b1db
{
    public static $prefixLengthsPsr4 = array (
        'D' => 
        array (
            'DiscordCoreAPI\\' => 4,
        ),
    );

    public static $prefixDirsPsr4 = array (
        'DiscordCoreAPI\\' => 
        array (
            0 => __DIR__ . '/../..' . '/Classes',
        ),
    );

    public static $classMap = array (
        'Composer\\InstalledVersions' => __DIR__ . '/..' . '/Composer/InstalledVersions.php',
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixLengthsPsr4 = ComposerStaticInit0e8415491642f27914717986db49b1db::$prefixLengthsPsr4;
            $loader->prefixDirsPsr4 = ComposerStaticInit0e8415491642f27914717986db49b1db::$prefixDirsPsr4;
            $loader->classMap = ComposerStaticInit0e8415491642f27914717986db49b1db::$classMap;

        }, null, ClassLoader::class);
    }
}
