<?php
/**
 * Views Functions.
 */

require_once dirname(__FILE__) . '/functions.inc';

/**
 * Views Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_views($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'View Name';
  $headers[1] = 'View Machine Name';
  $headers[2] = 'Base';
  $headers[3] = 'Description';
  $headers[4] = 'Display Type';
  $headers[5] = 'Display Type Name';
  $headers[6] = 'Display Type Machine name';
  $headers[7] = 'Display Type Title';
  $headers[8] = 'Filters';
  $headers[9] = 'Sort';
  $headers[10] = 'Ctx Filters';
  $headers[11] = 'Relationship';
  $headers[12] = 'Style / Format';
  $headers[13] = 'Empty Text';
  $headers[14] = 'Paging';
  $headers[15] = 'Access Control';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load up all menus.
  $views = views_get_all_views();

  foreach ($views as $key => $view) {
    // Go through and add each view, first row is default / master.
    if (isset($view->display)) {
      foreach ($view->display as $display) {

        // View Filters.
        $filters = '';
        if (isset($display->display_options['filters'])) {
          $filters = _kala_migrate_get_field_settings($display->display_options['filters']);
        }

        // View Sorts.
        $sorts = '';
        if (isset($display->display_options['sorts'])) {
          $sorts = _kala_migrate_get_field_settings($display->display_options['sorts']);
        }

        // Contextual Filters.
        $context = '';
        if (isset($display->display_options['arguments'])) {
          $context = _kala_migrate_get_field_settings($display->display_options['arguments']);
        }

        // Relationships.
        $relationship = '';
        if (isset($display->display_options['relationships'])) {
          $relationship = _kala_migrate_get_field_settings($display->display_options['relationships']);
        }

        // Display / Format.
        $style = '';
        if (isset($display->display_options['style_plugin'])) {
          $plugin = $display->display_options['style_plugin'];
          if ($plugin == 'default') {
            $style = 'Unformatted List';
          }
          else {
            $style = ucwords(str_replace('_', ' ', $plugin));
          }
        }

        // Empty Text
        $empty = '';
        if (isset($display->display_options['empty'])) {
          $empty_text = $display->display_options['empty'];
          foreach ($empty_text as $empty_content) {
            $empty = isset($empty_content['content']) ?
                     $empty_content['content'] : '';
          }
        }

        // Pager
        $paging = '';
        if (isset($display->display_options['pager'])) {
          $pager = $display->display_options['pager'];
          // go through and get the opton for the pager type.
          if (isset($pager['type']) && $pager['type'] != 'none') {
            $paging .= $pager['type'] . ': ';
            if (isset($pager['options'])) {
              $paging .= _kala_migrate_get_field_settings($pager['options']);
            }
          }
          // No pager.
          else {
            $paging .= $pager['type'];
          }
        }

        // View Permisisons.
        $perm = '';
        if (isset($display->handler->view->display[$display->id])) {
          foreach ($display->handler->view->display[$display->id] as $handler) {
            // Check for object cause this will implode if you don't.
            if (!is_object($handler) && isset($handler['access']['type'])) {
              // Set up the string with condition.
              $type = $handler['access']['type'];
              $perm .= $type == 'perm' ? 'Permission: ' : ucwords($type) . ': ';
              if (isset($handler['access']['perm'])) {
                $perm .= ucwords($handler['access']['perm']);
              }
            }
          }
        }

        if ($display->id == 'default') {
          $body['View Name'] = $view->human_name;
          $body['View Machine Name'] = $view->name;
          $body['Base'] = $view->base_table;
          $body['Description'] = $view->description;
          $body['Display Type'] = $display->display_title;
          $body['Display Type Name'] = '';
          $body['Display Type Machine name'] = 'N/A';
          $body['Display Type Title'] = '';
          $body['Filters'] = $filters;
          $body['Sort'] = $sorts;
          $body['Ctx Filters'] = $context;
          $body['Relationship'] = $relationship;
          $body['Style / Format'] = $style;
          $body['Empty Text'] = $empty;
          $body['Paging'] = $paging;
          $body['Access Control'] = $perm;
        }
        else {
          $body['View Name'] = '';
          $body['View Machine Name'] = '';
          $body['Base'] = '';
          $body['Description'] = '';
          $body['Display Type'] = $display->display_plugin;
          $body['Display Type Name'] = $display->display_title;
          $body['Display Type Machine name'] = $display->id;
          $body['Display Type Title'] = $display->display_title;
          $body['Filters'] = $filters;
          $body['Sort'] = $sorts;
          $body['Ctx Filters'] = $context;
          $body['Relationship'] = $relationship;
          $body['Style / Format'] = $style;
          $body['Empty Text'] = $empty;
          $body['Paging'] = $paging;
          $body['Access Control'] = $perm;
        }

        fputcsv($f, $body);
      }
    }


    // throw it in the file meow.
    // fputcsv($f, $body);

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
