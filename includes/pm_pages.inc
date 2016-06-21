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

  $headers[0] = 'PM Name';
  $headers[1] = 'PM Machine Name';
  $headers[2] = 'Module';
  $headers[3] = 'Description';
  $headers[3] = 'Path';

  // print headers to CSV
  fputcsv($f, $headers);

  // Grab all the plugins to use later.
  foreach (ctools_plugin_get_plugin_type_info() as $module => $info) {
    // Go through each plugin to see if we can load it.
    foreach (array_keys($info) as $id_type) {
      $plugins = ctools_get_plugins($module, $id_type);
      // See if there is a schema.
      foreach ($plugins as $pkey => $plugin) {
        if (isset($plugin['schema'])) {
          $types[$pkey] = $plugin['schema'];
        }
      }
    }
  }

  // Grab all the tasks and lets give this a whirl.
  $tasks = page_manager_get_tasks();
  foreach ($tasks as $key => $task) {
    $type = '';
    // See if there is a schema to load the object.
    $name = $task['name'];
    foreach ($types as $tkey => $tvalue) {
      if ($name == $tkey) {
        $type = $tvalue;
        break;
      }
    }
    // Fix for Page Manager and set type for the rest.
    if (empty($type)) {
      $type = $task['name'] == 'page' ? 'page_manager_pages' : $task['name'];
    }

    // Grab all the pages of the type.
    $pages = ctools_export_load_object($type);
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

        $module = str_replace('_', ' ', $page->table);
        if (stripos($module, '_pages') !== FALSE) {
          $module = strstr($module, '_pages');
        }

        $body['PM Name'] = $page->admin_title;
        $body['PM Machine Name'] = $page->name;
        $body['Module'] = ucwords($module);
        $body['Description'] = isset($page->admin_description) ? $page->admin_description : '';
        $body['Path'] = isset($page->path) ? $page->path : '';

        fputcsv($f, $body);

        // if (isset($page->default_handlers)) {
        //   foreach ($page->default_handlers as $key => $variant) { }
        // }
        // dpm($page);

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
