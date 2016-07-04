<?php
/**
 * @file
 * Kala Migrate CSV Export Tab UI & Functions.
 */

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
