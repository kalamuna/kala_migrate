# Kala Migrate

**This module only works if you can write to sites/all/modules.  I would suggest just using it locally**

Help with reporting of items that need to be migrated in a Drupal 7 to Drupal 8 upgrade.

Configuration page can be found at: admin/config/development/kala_migrate

## CSV Exports

This page will scan through and give you an inventory of all the structures that are enabled.

The CSV Exports Outputs a ZIP file that contains CSV's in utf-8 chatcter set.  Make sure you change your character set encoding to match if using LibreCalc.

## Module Page

The Module Page uses cURL when you press the fancy button.  It shoots out numerous cURL requests, so make sure your host or local can handle these things.

This page will give you displays all enabled modules (contrib, core, custom & features) + for contrib & core it displays their d8 status + the issue link for it if it exists.

It is color coded to help your eyes not bleed.

## Create your own (aka Plugins)

To add in your own set of checks for this module:

1. Create a file in the /includes/csv_functions folder
2. In the /includes/csv_functions/YOURFILE.inc file add this:

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

3. in the _kala_migrate_csv_page_form() (in includes/export_ui/admin_csv.inc) add a checkbox like:
```
$form['FIELDSET']['FIELD_NAME'] = array(
  '#type' => 'checkbox',
  '#title' => t('BLAH BLAH BLAH'),
  '#default_value' => 1,
);
```

4. In the _kala_migrate_csv_page_form_submit (in includes/export_ui/admin_csv.inc) add a check and link to function, with the file name:
```
if ($values['FIELD_NAME']) {
  $files[] = _kala_migrate_FUNCTION_NAME('FIELD_NAME.csv');
}
```

FIN!

## Functions

There is a function.inc file that contains some handy tools to display data.

### The Functions

```
_kala_migrate_pathauto_default_pattern($type)
```
Pass In the machine name of the type and it returns the pathauto pattern for that type.

```
_kala_migrate_pathauto_overidden_pattern($machine_name, $default_pattern)
```
Pass in the machine name + the default pattern from 1 and it will return the pathauto pattern if it is overridden.

```
_kala_migrate_get_field_settings($widget)
```
The most used function in this file.  This is for flattening an array of widget / entity settings into a string.  Pass in whatever settings array you are trying to get all the settings for.

