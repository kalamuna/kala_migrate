<?php
/**
 * Image Styles Functions.
 */

/**
 * Image Styles Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
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
    // Grab the first row to make this format happier.
    reset($style['effects']);
    $first = key($style['effects']);
    foreach ($style['effects'] as $key => $effect) {
      // Grab the settings / effects.
      $settings = '';
      foreach ($effect['data'] as $setting => $data) {
        $settings .= '"' . $setting . ' = ' . $data . '", ';
      }

      // Set up first row and then tab the rest.
      if ($key === $first) {
        // Go thrugh and add all the styles.
        $body['Style Name'] = $style['label'];
        $body['Style Machine Name'] = $style['name'];
        $body['Effect'] = $effect['label'];
        $body['Settings'] = $settings;
      }
      else {
        $body['Style Name'] = '';
        $body['Style Machine Name'] = '';
        $body['Effect'] = $effect['label'];
        $body['Settings'] = $settings;
      }

      // throw it in the file meow.
      fputcsv($f, $body);
    }

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
