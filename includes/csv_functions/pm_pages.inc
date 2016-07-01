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
  $headers[3] = 'Enabled?';
  $headers[4] = 'Description';
  $headers[5] = 'Path';
  $headers[6] = 'Access';
  $headers[7] = 'Menu';
  $headers[8] = 'Variant Name';
  $headers[9] = 'Variant Machine Name';

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
        // Fix up the module namefor styling wins.
        $module = str_replace('_', ' ', $page->table);
        if (stripos($module, '_pages') !== FALSE) {
          $module = strstr($module, '_pages');
        }

        // See if this is enabled or not.
        if (isset($page->disabled)) {
          $enabled = $page->disabled ? 'No' : 'Yes';
        }
        else {
          $enabled = 'Yes';
        }

        // Access Settings.
        $access = '';
        if (isset($page->access)) {
          $access = _kala_migrate_get_field_settings($page->access);
        }

        // Menu Settings.
        $menu = '';
        if (isset($page->menu)) {
          $menu = _kala_migrate_get_field_settings($page->menu);
        }

        // Load the handlers
        $handlers = isset($page->default_handlers) ? $page->default_handlers :
                    page_manager_load_sorted_handlers($task, $page->name);

        // If there are variants, lets do this.
        if (isset($handlers)) {
          // Grab the first key for tabbing.
          reset($handlers);
          $first = key($handlers);

          // Load each variant up
          foreach ($handlers as $key => $variant) {
            // Set up first row and then tab the rest.
            if ($key === $first) {
              $body['PM Name'] = isset($page->admin_title) ? $page->admin_title : $page->label;
              $body['PM Machine Name'] = $page->name;
              $body['Module'] = ucwords($module);
              $body['Enabled?'] = $enabled;
              $body['Description'] = isset($page->admin_description) ? $page->admin_description : '';
              $body['Path'] = isset($page->path) ? $page->path : '';
              $body['Access'] = $access;
              $body['Menu'] = $menu;
              $body['Variant Name'] = $variant->conf['title'];
              $body['Variant Machine Name'] = $key;
            }
            else {
              $body['PM Name'] = '';
              $body['PM Machine Name'] = '';
              $body['Module'] = '';
              $body['Enabled?'] = '';
              $body['Description'] = '';
              $body['Path'] = '';
              $body['Access'] = '';
              $body['Menu'] = '';
              $body['Variant Name'] = $variant->conf['title'];
              $body['Variant Machine Name'] = $key;
            }

            // Throw it all in now.
            fputcsv($f, $body);
          }
        }
        else {
          // No Variants.
          $body['PM Name'] = isset($page->admin_title) ? $page->admin_title : $page->label;
          $body['PM Machine Name'] = $page->name;
          $body['Module'] = ucwords($module);
          $body['Enabled?'] = $enabled;
          $body['Description'] = isset($page->admin_description) ? $page->admin_description : '';
          $body['Path'] = isset($page->path) ? $page->path : '';
          $body['Access'] = $access;
          $body['Menu'] = $menu;
          $body['Variant Name'] = '';
          $body['Variant Machine Name'] = '';

          // Throw it all in now.
          fputcsv($f, $body);
        }
      }
    }
    else {
      // These are non page manager types that are task based.
      ctools_include('context');
      ctools_include('context-task-handler');
      $handlers = page_manager_load_sorted_handlers($task, '', TRUE);

      // See if this is enabled or not.
      if (isset($task->disabled)) {
        $enabled = $task->disabled ? 'No' : 'Yes';
      }
      else {
        $enabled = 'Yes';
      }

      // Get the access call back hook.
      $access = isset($task['access callback']) ?
                'Callback: ' . $task['access callback'] : '';


      if (isset($handlers)) {
        // Grab the first key for tabbing.
        reset($handlers);
        $first = key($handlers);

        // Load each variant up
        foreach ($handlers as $key => $variant) {
          // Set up first row and then tab the rest.
          if ($key === $first) {
            $body['PM Name'] = $task['title'];
            $body['PM Machine Name'] = $task['name'];
            $body['Module'] = ucwords(str_replace('_', ' ', $task['module']));
            $body['Enabled?'] = $enabled;
            $body['Description'] = isset($task['admin_description']) ? $task['admin_description'] : '';
            $body['Path'] = isset($task['admin path']) ? $task['admin path'] : '';
            $body['Access'] = $access;
            $body['Menu'] = '';
            $body['Variant Name'] = $variant->conf['title'];
            $body['Variant Machine Name'] = $key;
          }
          else {
            $body['PM Name'] = '';
            $body['PM Machine Name'] = '';
            $body['Module'] = '';
            $body['Enabled?'] = '';
            $body['Description'] = '';
            $body['Path'] = '';
            $body['Access'] = '';
            $body['Menu'] = '';
            $body['Variant Name'] = $variant->conf['title'];
            $body['Variant Machine Name'] = $key;
          }

          // Throw it all in now.
          fputcsv($f, $body);
        }
      }
      else {
        // No Variants.
        $body['PM Name'] = $task['title'];
        $body['PM Machine Name'] = $task['name'];
        $body['Module'] = ucwords(str_replace('_', ' ', $task['module']));
        $body['Enabled?'] = $enabled;
        $body['Description'] = isset($task['admin_description']) ? $task['admin_description'] : '';
        $body['Path'] = isset($task['admin path']) ? $task['admin path'] : '';
        $body['Access'] = $access;
        $body['Menu'] = '';
        $body['Variant Name'] = '';
        $body['Variant Machine Name'] = '';

        // Throw it all in now.
        fputcsv($f, $body);
      }
    }
  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
