<?php
/**
 * @file
 * Kala Migrate Module Status Tab UI & Functions.
 */

/**
 * Module Export Page Callback.
 */
function kala_migrate_module_page() {
  // Add the css
  $module = drupal_get_path('module', 'kala_migrate');
  drupal_add_css($module . '/css/kala-migrate.css');

  // Grab filtered enabled modules list.
  $modules = _kala_migrate_get_modules();

  // Begin Tables Markup
  $tables = '';

  // Filter the module respectively for happy table factoring.
  foreach ($modules as $module) {
    switch ($module->kala_migrate['module-type']) {
      case 'Contrib':
        $contrib[] = _kala_migrate_module_filter($module->name, $module->kala_migrate);
        break;
      case 'Core':
        $core[] = _kala_migrate_module_filter($module->kala_migrate['module-type'], $module->kala_migrate);
        break;
      case 'Custom':
        $custom[] = _kala_migrate_module_filter($module->kala_migrate['module-type'], $module->kala_migrate);
        break;
      case 'Feature':
        $features[] = _kala_migrate_module_filter($module->kala_migrate['module-type'], $module->kala_migrate);
        break;
    }
  }

  // Generate Contrib Table.
  if (isset($contrib)) {
    $tables .= '<h2>' . t('CONTRIB MODULES') . '</h2>';
    $tables .= _kala_migrate_generate_table($contrib, 'Contrib');
  }

  // Grab Core Table.
  if (isset($core)) {
    $tables .= '<h2>' . t('CORE MODULES') . '</h2>';
    $tables .= _kala_migrate_generate_table($core, 'Core');
  }

  // Grab Custom Table.
  if (isset($custom)) {
    $tables .= '<h2>' . t('CUSTOM MODULES') . '</h2>';
    $tables .= _kala_migrate_generate_table($custom, 'Custom');
  }

  // Grab Features Table.
  if (isset($features)) {
    $tables .= '<h2>' . t('FEATURES') . '</h2>';
    $tables .= _kala_migrate_generate_table($features, 'Feature');
  }

  // Grab the refresh form button.
  $form = drupal_get_form('kala_migrate_module_form');

  $page = array(
    '#prefix' => '<center>',
    'form' => $form,
    'table' => array(
      '#markup' => $tables,
    ),
    '#suffix' => '</center>',
  );

  return $page;
}

/**
 * Module Export Form.
 */
function kala_migrate_module_form() {
  $form = array();
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh Module Table Data'),
    '#submit' => array('kala_migrate_module_form_submit'),
  );
  return $form;
}

/**
 * Submit handler for csv exports form.
 */
function kala_migrate_module_form_submit() {
  // Fill in table initially and after the fact if need be.
  $sql = 'SELECT name, info, kala_migrate from {system} WHERE type = :type';
  $query = db_query($sql, array(':type' => 'module'));

  // Set up insert if need be and fill in the fields.
  foreach ($query->fetchAll() as $record) {
    if (!isset($record->kala_migrate)) {
      $data = array(
        'name' => $record->name,
        'kala_migrate' => $record->info,
      );
      // Is there a way to write this outside of the foreach?
      drupal_write_record('system', $data, 'name');
    }
  }

  // Grab filtered enabled modules list.
  $modules = _kala_migrate_get_modules();

  foreach ($modules as $module) {
    if (!$module->kala_migrate['skip']) {
      $operations[] = array('kala_migrate_batch', array($module->name, $module->kala_migrate));
    }
  }

  // Set Batch fun stuff.
  $batch = array(
    'title' => t('Assimilating...'),
    'operations' => $operations,
    'finished' => 'kala_migrate_batch_finished',
  );

  // Engage.
  batch_set($batch);
}

/**
 * Fetch and return object of non duplicate modules.
 *
 * @return object
 *   Modules list.
 */
function _kala_migrate_get_modules() {
  // Now grab only active modules.
  $sql = 'SELECT name, kala_migrate from {system} WHERE type = :type AND status = :status ORDER BY name ASC';
  $query = db_query($sql, array(':type' => 'module', ':status' => 1));

  $modules = array();
  // Go through and remove broken project and other non used things.
  foreach ($query->fetchAll() as $module) {
    if (isset($module->kala_migrate)) {
      $info = unserialize($module->kala_migrate);
      $output = new stdClass();
      if (isset($info['project']) && !isset($info['features'])) {
        $info['module-type'] = 'Contrib';
        $info['skip'] = FALSE;
        // Check if submodule.
        if (isset($info['dependencies'])) {
          foreach ($info['dependencies'] as $deps) {
            if ($deps == $info['project'])  {
              $info['submodule'] = $info['project'];
            }
          }
        }
        // Set the object back up.
        $output->name = $module->name;
        $output->kala_migrate = $info;
      }
      else {
        // Add in special project info for core for later use.
        $info = unserialize($module->kala_migrate);
        if (isset($info['package']) && $info['package'] == 'Core') {
          $info['module-type'] = 'Core';
          $info['skip'] = FALSE;
        }
        elseif (isset($info['features'])) {
          $info['module-type'] = 'Feature';
          $info['skip'] = TRUE;
        }
        else {
          $info['module-type'] = 'Custom';
          $info['skip'] = TRUE;
        }
        // Set the object back up.
        $output->name = $info['module-type'];
        $output->kala_migrate = $info;
      }
      $modules[] = $output;
    }
  }

  return $modules;
}

/**
 * Filter and rebuild object for Module based on module-type
 *
 * @param string $name
 *  Project or Module Type name
 *
 * @param object $info
 *   The info BLOB.
 *
 * @return object
 *   Filtered newly formatted object.
 */
function _kala_migrate_module_filter($name, $info) {
  $output = new stdClass();
  $output->name = $name;
  $output->kala_migrate = $info;
  return $output;
}

/**
 * Generate Table Layout for the module page.
 *
 * @param object $modules
 *   Array of module object from the DB.
 *
 * @param string $type
 *  Module classification identifier.
 *
 * @return string
 *   HTML formatted table.
 */
function _kala_migrate_generate_table($modules, $type) {

  // Table Markup.
  $table  = '<table id="kala-migrate">';
  $table .= '<thead>';
  $table .= '<tr>';

  // Different column layout based on types.
  switch ($type) {
    case 'Contrib':
      $table .= '<th>Module Name</th>';
      $table .= '<th>Module Link</th>';
      $table .= '<th>D8 Status</th>';
      $table .= '<th>D8 Module Version</th>';
      $table .= '<th>D8 Issue Progress Link</th>';
      break;

    case 'Core':
      $table .= '<th>Module Name</th>';
      $table .= '<th>Module Link</th>';
      $table .= '<th>D8 Status</th>';
      $table .= '<th>D8 Issue Progress Link</th>';
      break;

    // Custom or Feature.
    default:
      $table .= '<th>Module Name</th>';
      $table .= '<th>Module Type</th>';
      break;
  }

  $table .= '</tr>';
  $table .= '</thead>';
  $table .= '<tbody>';

  // Go through and lets do this.
  foreach ($modules as $module) {
    // Set info to var for easier using.
    $info = $module->kala_migrate;

    // Go through and grab status for class.
    $class = _kala_migrate_css_class($info, $type);

    // Opening row with fancy class.
    $table .= '<tr ' . $class . '>';

    // Name of thy module..
    $table .= '<td>' . $info['name'] . '</td>';

    // Go through and format based on type.
    switch ($type) {
      // Contrib modules.
      case 'Contrib':
        if (!isset($info['submodule'])) {
          // Project Link or Special.
          $url = 'http://www.drupal.org/project/' . $info['project'];
          $url = l($url, check_url($url),
            array('attributes' => array('target'=>'_blank'))
          );
          $table .= '<td>' . $url . '</td>';

          // Drupal 8 status.
          $status = isset($info['drupal8status']) ? $info['drupal8status'] : '';
          $table .= '<td>' . $status . '</td>';

          $version = isset($info['drupal8ver']) ? $info['drupal8ver'] : '';
          $table .= '<td>' . $version . '</td>';

          // Drupal 8 Status Link.
          if (isset($info['drupal8issue'])) {
            if (stripos($info['drupal8issue'], 'http://') !== FALSE) {
              $link = l($info['drupal8issue'], check_url($info['drupal8issue']),
                      array('attributes' => array('target'=>'_blank')));
            }
            else {
              $link = $info['drupal8issue'];
            }
          }
          else {
            $link = 'NONE';
          }
          $table .= '<td>' . $link . '</td>';
        }
        else {
          $table .= '<td colspan="4"><center>Submodule of ' . $info['submodule'] . '</center></td>';
        }
        break;

      case 'Core':
        // Project Link or Special.
        $table .= '<td>CORE</td>';

        // Drupal 8 status.
        $status = isset($info['drupal8status']) ? $info['drupal8status'] : 'IN CORE';
        $table .= '<td>' . $status . '</td>';

        // Drupal 8 Status Link.
        if (isset($info['drupal8issue'])) {
          if (stripos($info['drupal8issue'], 'http://') !== FALSE) {
            $link = l($info['drupal8issue'], check_url($info['drupal8issue']),
                    array('attributes' => array('target'=>'_blank')));
          }
          else {
            $link = $info['drupal8issue'];
          }
        }
        else {
          $link = 'IN CORE';
        }
        $table .= '<td>' . $link . '</td>';
        break;

      // Custom or Feature.
      default:
        // Project Link or Special.
        $table .= '<td>'. $type . '</td>';
    }

    $table .= '</tr>';
  }

  $table .= '</tbody>';
  $table .= '</table>';
  return $table;
}

/**
 * Generates CSS class for table row & cell.
 *
 * @param object $info
 *   The info BLOB.
 *
 * @param string $type
 *  Module classification identifier.
 *
 * @return string
 *   The class or empty..
 */
function _kala_migrate_css_class($info, $type) {
  $class = $class_apply = '';
  if (isset($info['drupal8status']) && $type == 'Contrib') {
    // Lets check whats up
    switch ($info['drupal8status']) {
      case 'IN CORE':
        $class_apply = 'core';
        break;

      case 'NOT PORTED YET':
        $class_apply = 'not-ported';
        break;

      case 'RENAMED OR DEPRECIATED':
        $class_apply = 'removed';
        break;

      case 'IN PROGRESS':
        $class_apply = 'inprogress';
        break;

      case 'PORTED':
        $class_apply = 'ported';
        break;
    }
    // Set up the css fun.
    if (isset($class_apply) && !empty($class_apply)) {
      $class = ' class="' . $class_apply . '"';
    }
  }
  elseif (isset($info['submodule'])) {
    $class = ' class="submodule"';
  }

  return $class;
}
