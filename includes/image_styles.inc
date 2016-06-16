<?php
/**
 * Image Styles Functions.
 */

/**
 * Image Styles Processing function.
 *
 * @param  string $filename file name
 * @return string       file name
 */
function _kala_migrate_image_styles($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Style Name';
  $headers[1] = 'Style Machine Name';
  $headers[2] = 'Effect';
  $headers[3] = 'Settings';

  // print headers to CSV
  fputcsv($f, $headers);
  $styles = image_styles();

  foreach ($styles as $style) {
    $body['Style Name'] = $style['label'];
    $body['Style Machine Name'] = $style['name'];
    $body['Effect'] = '';
    $body['Settings'] = '';

    // throw it in the file meow.
    fputcsv($f, $body);

    // Blank Columns To tab this sort of.
    $body['Style Name'] = '';
    $body['Style Machine Name'] = '';
    foreach ($style['effects'] as $effect) {
      $body['Effect'] = $effect['label'];
      $settings = '';
      foreach ($effect['data'] as $key => $data) {
        $settings .= '"' . $key . ' = ' . $data . '", ';
      }
      $body['Settings'] = $settings;

      // throw it in the file meow.
      fputcsv($f, $body);
    }

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
