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
              'mf-program-dialog.c',
              'mf-window.c' ] + resources,
            c_args: ['-DLOCALE_DIR="@0@"'.format (localedir)],
            install: true,
            dependencies: [ glib_dep, gio_dep, gudev_dep, json_glib_dep, gtk_dep ])
