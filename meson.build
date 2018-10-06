project ('microflash', 'c',
          meson_version: '>= 0.37.1')

gnome = import ('gnome')

glib_dep = dependency ('glib-2.0')
gio_dep = dependency ('gio-2.0')
gudev_dep = dependency ('gudev-1.0')
json_glib_dep = dependency ('json-glib-1.0')
gtk_dep = dependency ('gtk+-3.0')

resources = gnome.compile_resources(
  'mf-resources',
  'microflash.gresource.xml',
  source_dir : '.',
  c_name : 'mf'
)

executable ('microflash',
            [ 'microflash.c',
              'mb-device.c',
              'mb-file.c',
              'mb-monitor.c',
              'mf-window.c' ] + resources,
            dependencies: [ glib_dep, gio_dep, gudev_dep, json_glib_dep, gtk_dep ])


