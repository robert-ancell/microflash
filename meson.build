project ('microflash', 'c',
          meson_version: '>= 0.37.1')

gnome = import ('gnome')

glib_dep = dependency ('glib-2.0')
gio_dep = dependency ('gio-2.0')
gudev_dep = dependency ('gudev-1.0')

executable ('microflash',
            [ 'microflash.c',
              'mb-device.c',
              'mb-monitor.c' ],
            dependencies: [ glib_dep, gio_dep, gudev_dep ])



