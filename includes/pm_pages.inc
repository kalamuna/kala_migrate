<?php
/**
 * Page Manager Pages Functions.
 */

/**
 * Page Manager Pages Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_pm_pages($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Menu Name';
  $headers[1] = 'Menu Machine Name';

  // print headers to CSV
  fputcsv($f, $headers);

  // Grab all the tasks and lets give this a whirl.
  $tasks = page_manager_get_tasks();
  foreach ($tasks as $key => $task) {
    // See if there is a schema to load the object.
    $plugin = ctools_get_plugins('ctools', 'export_ui', $task['module']);
    $load = isset($plugin['schema']) ? $plugin['schema'] : $task['name'];
    // Fix for Page Manager cause yes.
    if ($load == 'page') {
      $load = 'page_manager_pages';
    }

    $pages = ctools_export_load_object($load);
    if ($pages) {
      // Grab the path to use for the include.
      $path = strstr($task['path'], $task['module']);
      $path = str_replace($task['module'] . '/', '', $path);
      $file = strstr($task['file'], '.inc', TRUE);
      $inc = $path . '/' . $file;
      // Load each file we need for this.
      module_load_include('inc', $task['module'], $inc);

      // Lets go through and load this up.
      foreach ($pages as $page) {
        // dpm($page);
    // Load Variants and such.
    // if (isset($page->default_handlers)) {
    //   foreach ($page->default_handlers as $key => $variant) { }
    // // $body['Menu Name'] = $menu_name;
    // // $body['Menu Machine Name'] = $key;

    // // // throw it in the file meow.
    // // fputcsv($f, $body);
    // }
      }
    }
    else {
      // LOAD THE NON PAGE MAAER TYPE LIKE NODE VIEW JUST IN THE CSV.
    }
  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
