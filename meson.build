project ('microflash', 'c',
          meson_version: '>= 0.37.1')

gnome = import ('gnome')
i18n = import ('i18n')

datadir = join_paths (get_option ('prefix'), get_option ('datadir'))
localedir = join_paths (get_option ('prefix'), get_option ('localedir'))

glib_dep = dependency ('glib-2.0')
gio_dep = dependency ('gio-2.0')
gudev_dep = dependency ('gudev-1.0')
json_glib_dep = dependency ('json-glib-1.0')
gtk_dep = dependency ('gtk+-3.0')

i18n.merge_file ('appdata-file',
                 input: 'microflash.appdata.xml',
                 output: 'microflash.appdata.xml',
                 install: true,
                 install_dir: join_paths (datadir, 'metainfo'),
                 po_dir: 'po')

i18n.merge_file ('desktop-file',
                 input: 'microflash.desktop',
                 output: 'microflash.desktop',
                 install: true,
                 install_dir: join_paths (datadir, 'applications'),
                 po_dir: 'po',
                 type: 'desktop')

i18n.merge_file ('mime-file',
                 input: 'microflash.mime.xml',
                 output: 'microflash.xml',
                 install: true,
                 install_dir: join_paths (datadir, 'mime', 'packages'),
                 po_dir: 'po',
                 data_dirs: '.',
                 type: 'xml')

install_data ('microflash.png',
              install_dir: join_paths (datadir, 'icons', 'hicolor', '256x256', 'apps'))

subdir ('po')
subdir ('src')
