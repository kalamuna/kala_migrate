<?php
/**
 * @file
 * Kala Migrate and module settings UI.
 */

/**
 * Info Page Callback.
 */
function kala_migrate_info() {
  $html  = '<h2>' . t('Kala Migrate Options') . '</h2>';
  $html .= '<ol>';
  $html .= '<li>' . t('<b>EXPORT CSV:</b> Export all structural and architectural items for the site into multiple csv\'s contained in ZIP file.') . '</li>';
  $html .= '<li>' . t('<b>MODULES STATUS:</b> Module list for exporting to D8.') . '</li>';
  $html .= '</ol>';

  return array('#markup' => $html);
}

/**
 * CSV Exports Form / Page Callback.
 */
function kala_migrate_settings_form() {

  $form['ct'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Based Items'),
  );

  $form['ct']['content_types'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get all Content Types and their data (includes some field data)'),
    '#default_value' => 1,
  );

  $form['ct']['fields'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get full Fields data (more detailed)'),
    '#default_value' => 1,
  );

  $form['tx'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy Based Items'),
  );

  $form['tx']['vocab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get all of the Taxonomy Vocabulary Settings?'),
    '#default_value' => 1,
  );

  $form['ps'] = array(
    '#type' => 'fieldset',
    '#title' => t('Panels Based Items'),
  );

  if (module_exists('page_manager')) {
    $form['ps']['pm_pages'] = array(
      '#type' => 'checkbox',
      '#title' => t('Get all of the Page Manager Pages and their Settings?'),
      '#default_value' => 1,
    );
  }

  if (module_exists('panelizer')) {
    $form['ps']['pzr'] = array(
      '#type' => 'checkbox',
      '#title' => t('Get a list of Panelizer Entities.'),
      '#default_value' => 1,
    );
  }

  if (module_exists('fieldable_panels_panes')) {
    $form['ps']['fpp'] = array(
      '#type' => 'checkbox',
      '#title' => t('Get all of Fieldable Panels Panes?'),
      '#default_value' => 1,
    );
  }

  if (module_exists('panels_mini')) {
    $form['ps']['mini'] = array(
      '#type' => 'checkbox',
      '#title' => t('Get all of the Mini Panels and their Settings?'),
      '#default_value' => 1,
    );
  }

  $form['is'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image Based Items'),
  );

  $form['is']['image_styles'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get all Image Styles and Settings?'),
    '#default_value' => 1,
  );

  $form['vs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Views Based Items'),
  );

  $form['vs']['views'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get all Views and their Settings?'),
    '#default_value' => 1,
  );

  $form['am'] = array(
    '#type' => 'fieldset',
    '#title' => t('Admin Based Items'),
  );

  if (module_exists('features')) {
    $form['am']['feat'] = array(
      '#type' => 'checkbox',
      '#title' => t('Get all of the Features and their Settings?'),
      '#default_value' => 1,
    );
  }

  $form['am']['menus'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get all of the Menu Settings?'),
    '#default_value' => 1,
  );

  $form['am']['roles'] = array(
    '#type' => 'checkbox',
    '#title' => t('Get all of the User Roles?'),
    '#default_value' => 1,
  );

  if (module_exists('redirect')) {
    $form['am']['redirect'] = array(
      '#type' => 'checkbox',
      '#title' => t('Get all of the Redirects?'),
      '#default_value' => 1,
    );
  }

  $form['#submit'][] = 'kala_migrate_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Submit handler for csv exports form.
 */
function kala_migrate_settings_form_submit($form, &$form_state) {
  $files = array();
  $values = $form_state['values'];

  // The Content Type & Fields Settings.
  if ($values['content_types']) {
    $files[] = _kala_migrate_content_types('content-types.csv');
  }

  // The Full Fields Settings.
  if ($values['fields']) {
    $files[] = _kala_migrate_fields('fields.csv');
  }

  // The Taxonomy Vocabulary Settings.
  if ($values['vocab']) {
    $files[] = _kala_migrate_vocab('vocab.csv');
  }

  // The Page Manager Pages.
  if (module_exists('page_manager')) {
    if ($values['pm_pages']) {
      $files[] = _kala_migrate_pm_pages('page-manager-pages.csv');
    }
  }

  if (module_exists('panelizer')) {
    if ($values['pzr']) {
      $files[] = _kala_migrate_panelizer('panelizer.csv');
    }
  }

  // The FPP Items.
  if (module_exists('fieldable_panels_panes')) {
    if ($values['fpp']) {
      $files[] = _kala_migrate_fpp('fieldable-panels-panes.csv');
    }
  }

  if (module_exists('panels_mini')) {
    if ($values['mini']) {
      $files[] = _kala_migrate_mini_panels('mini-panels.csv');
    }
  }

  // The Image Styles & Settings.
  if ($values['image_styles']) {
    $files[] = _kala_migrate_image_styles('image-styles.csv');
  }

  // The Views Settings.
  if ($values['views']) {
    $files[] = _kala_migrate_views('views.csv');
  }

  if (module_exists('features')) {
    if ($values['feat']) {
      $files[] = _kala_migrate_features('features.csv');
    }
  }

  // The Menus Settings.
  if ($values['menus']) {
    $files[] = _kala_migrate_menus('menus.csv');
  }

  // The Menus Settings.
  if ($values['roles']) {
    $files[] = _kala_migrate_user_roles('user-roles.csv');
  }

  // The Redirect Settings.
  if (module_exists('redirect')) {
    if ($values['redirect']) {
      $files[] = _kala_migrate_redirects('redirects.csv');
    }
  }

  // If there are files, lets do this!
  if (count($files) > 0) {
    _kala_migrate_create_zip($files);
  }

  // Get to the chopper!
  exit;
}

/**
 * Module Export Page Callback.
 */
function kala_migrate_module_page() {
  // Grab filtered enabled modules list.
  $modules = _kala_migrate_get_modules();

  // Table Markup.
  $table  = '<table border="1">';
  $table .= '<thead>';
  $table .= '<tr>';
  $table .= '<th>Module Name</th>';
  $table .= '<th>Module Link</th>';
  $table .= '<th>D8 Status</th>';
  $table .= '<th>D8 Issue Progress Link</th>';
  $table .= '</tr>';
  $table .= '</thead>';
  $table .= '<tbody>';
  foreach ($modules as $module) {
    // Set info to var for easier using.
    $info = $module->info;

    $table .= '<tr>';

    // Name.
    $table .= '<td>' . $info['name'] . '</td>';

    // Project Link or Special.
    $skip = array('Custom', 'Feature');
    if (!in_array($info['project'], $skip) && $info['project'] != 'Core') {
      $url = 'http://www.drupal.org/project/' . $info['project'];
      $url = l($url, check_url($url),
        array('attributes' => array('target'=>'_blank'))
      );
    }
    else {
      $url = $info['project'];
    }
    $table .= '<td>' . $url . '</td>';

    // Drupal 8 status.
    if (!in_array($info['project'], $skip)) {
      $status = isset($info['drupal8status']) ? $info['drupal8status'] : '';
    }
    else {
      $status = $info['project'];
    }
    $table .= '<td>' . $status . '</td>';

    // Drupal 8 Status Link.
    if (!in_array($info['project'], $skip)) {
      $link = isset($info['drupal8issue']) ?
              l($info['drupal8issue'], check_url($info['drupal8issue']),
                array('attributes' => array('target'=>'_blank'))) : '';
    }
    else {
      $link = 'N/A';
    }
    $table .= '<td>' . $link . '</td>';
    $table .= '</tr>';
  }

  $table .= '</tbody>';
  $table .= '</table>';

  // Grab the refresh form button.
  $form = drupal_get_form('kala_migrate_module_form');

  $page = array(
    '#prefix' => '<center>',
    'form' => $form,
    'table' => array(
      '#markup' => $table,
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
  // Grab filtered enabled modules list.
  $modules = _kala_migrate_get_modules();

  foreach ($modules as $module) {
    if (!$module->info['skip']) {
      $operations[] = array('kala_migrate_batch', array($module->name, $module->info));
    }
  }

  // Engage.
  $batch = array(
    'title' => t('Assimilating...'),
    'operations' => $operations,
    'finished' => 'kala_migrate_batch_finished',
  );

  batch_set($batch);
}


/**
 * Fetch and return object of non duplicate modules.
 *
 * @return object
 *   Modules list.
 */
function _kala_migrate_get_modules() {
  // Grab only active modules.
  $sql = 'SELECT name,info from {system} WHERE type = :type AND status = :status ORDER BY name ASC';
  $query = db_query($sql, array(':type' => 'module', ':status' => 1));

  $modules = array();
  // Go through and remove broken project and other non used things.
  foreach ($query->fetchAll() as $module) {
    if (isset($module->info)) {
      $info = unserialize($module->info);
      $output = new stdClass();
      if (isset($info['project']) && !isset($info['features'])) {
        $info['skip'] = FALSE;
        // Set the object back up.
        $output->name = $info['project'];
        $output->info = $info;
      }
      else {
        // Add in special project info for core for later use.
        $info = unserialize($module->info);
        if (isset($info['package']) && $info['package'] == 'Core') {
          $info['project'] = 'Core';
          $info['skip'] = FALSE;
        }
        elseif (isset($info['features'])) {
          $info['project'] = 'Feature';
          $info['skip'] = TRUE;
        }
        else {
          $info['project'] = 'Custom';
          $info['skip'] = TRUE;
        }
        // Set the object back up.
        $output->name = $info['project'];
        $output->info = $info;
      }
      $modules[] = $output;
    }
  }

  return $modules;
}
