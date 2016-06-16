# Kala Migrate

Help with reporting of items that need to be migrated in a Drupal upgrade.

Outputs a ZIP file that contains CSV's in utf-8 chatcter set.  Make sure you change your character set encoding to match if using LibreCalc.

## Create your own (aka Plugins)

To add in your own set of checks for this module:

1. Create a file in the /includes folder
2. Add this to the top of kala_migrate.module:
```
require_once dirname(__FILE__) . '/includes/YOURFILE.inc';
```

3. In the /includes/YOURFILE.inc file add this:
```
function _kala_migrate_FUNCTION_NAME($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Sample Header';

  // print headers to CSV
  fputcsv($f, $headers);

  // Logic goes here for body
  $body['Sample Header'] = 'whatever';
  fputcsv($f, $body);

  // Close this out.
  fclose($f);

  // Return File Name
  return $filename;
}
```

4. in the _kala_migrate_settings_form() add a checkbox like:
```
$form['FIELD_NAME'] = array(
  '#type' => 'checkbox',
  '#title' => t('BLAH BLAH BLAH'),
  '#default_value' => 1,
);
```

5. In the _kala_migrate_settings_form_submit add a check and link to function, with the file name:
```
if ($form_state['values']['FIELD_NAME']) {
  $files[] = _kala_migrate_FUNCTION_NAME('FIELD_NAME.csv');
}
```

FIN!
