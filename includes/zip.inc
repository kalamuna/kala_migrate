<?php
/**
 * Zip File Creation Functions.
 */

/**
 * Creates the Zip File.
 *
 * @param  array $files
 *  The Array of Files defined by other processes.
 */
function _kala_migrate_create_zip($files) {
  // File name for this fancy zip.
  $zipname = 'migrate.zip';

  // Instatiates PHP's Zip Archive Class.
  $zip = new ZipArchive;
  $zip->open($zipname, ZipArchive::CREATE);

  // Loop through all the files and add to zip.
  foreach ($files as $file) {
    $zip->addFile($file);
  }
  // All done!
  $zip->close();

  // Set headers and run with it.
  header('Content-Type: application/zip');
  header('Content-disposition: attachment; filename='.$zipname);
  header('Content-Length: ' . filesize($zipname));
  readfile($zipname);
}
