project ('microflash', 'c',
          meson_version: '>= 0.46.0')

gnome = import ('gnome')
i18n = import ('i18n')

datadir = join_paths (get_option ('prefix'), get_option ('datadir'))

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
              'mf-program-dialog.c',
              'mf-window.c' ] + resources,
            install: true,
            dependencies: [ glib_dep, gio_dep, gudev_dep, json_glib_dep, gtk_dep ])

i18n.merge_file ('desktop-file',
                 input: 'microflash.desktop.in',
                 output: 'microflash.desktop',
                 install: true,
                 install_dir: join_paths (datadir, 'applications'),
                 po_dir: 'po',
                 type: 'desktop')

i18n.merge_file ('mime-file',
                 input: 'microflash.mime.xml.in',
                 output: 'microflash.xml',
                 install: true,
                 install_dir: join_paths (datadir, 'mime', 'packages'),
                 po_dir: 'po',
                 data_dirs: '.',
                 type: 'xml')

subdir ('po')
