<?php

/**
 * Fields Functions.
 */

/**
 * Fields Processing function.
 *
 * @param  string $filename file name
 * @return string       file name
 */
function _kala_migrate_fields($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Field Name';
  $headers[1] = 'Field Machine Name';
  $headers[2] = 'Field Type';
  $headers[3] = 'Field Required?';
  $headers[4] = '# of Values Allowed';
  $headers[5] = 'Field Settings';
  $headers[6] = 'Default Value';
  $headers[7] = 'Help Text';

  // print headers to CSV
  fputcsv($f, $headers);
  // Grab all Content Types
  $types = node_type_get_types();
  foreach ($types as $type) {
    // Grab all the fields of the type.
    $fields = field_info_instances('node', $type->type);
    foreach ($fields as $field) {
      // Load additional field info.
      $info = field_info_field($field['field_name']);
      // Load in widget info for labels.
      $widget = field_info_widget_types($field['widget']['type']);

      // Add non standard field types per module, skip some.
      $dontadd = array('text', 'link', 'list', 'none');
      // Check for the Module
      $module = isset($field['display']['default']['module']) ?
                $field['display']['default']['module'] : 'none';

      // Add the non standard field type to the column output.
      $field_type = !in_array($module, $dontadd) ?
                    $module . ' ' . $widget['label'] : $widget['label'];

      // Grab the Number of Values allowed
      $nval = $info['cardinality'] == '-1' ? 'Unlimited' : $info['cardinality'];

      // Default Value
      $default = isset($field['default_value'][0]['value']) ?
                 $field['default_value'][0]['value'] : '';

      // Grab the settings if they exist.
      $settings = '';
      if (isset($field['widget']['settings'])) {
        foreach ($field['widget']['settings'] as $key => $value) {
          // Fixes file settings and other things like that.
          if (is_array($value)) {
            $flatvalue = '{';
            foreach ($value as $key2 => $value2) {
              $flatvalue .= '"' . $key2 . ' = ' . $value2 . '", ';
            }
            $value = rtrim($flatvalue, -2) . '}';
          }
          // Throw the settings into a happu string.
          $settings .= '"' . $key . ' = ' . $value . '", ';
        }
      }

      // Add the fields to the array to be added to the sheet.
      $body['Field Name'] = $field['label'];
      $body['Field Machine Name'] = $field['field_name'];
      $body['Field Type'] = ucwords($field_type);
      $body['Field Required?'] = $field['required'] ? 'Y' : 'N';
      $body['# of Values Allowed'] = $nval;
      $body['Field Settings'] = $settings;
      $body['Default Value'] = $default;
      $body['Help Text'] = $field['description'];

      // throw it in the file meow.
      fputcsv($f, $body);
    }
  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
