<?php
/**
 * Special Functions to use in the code base.
 */

/**
 * Returns the default pattern for type indicated.
 *
 * @param  string $type
 *   The type we are search for.
 *
 * @return string
 *   The default pattern for the type or NULL if none.
 */
function _kala_migrate_pathauto_default_pattern($type) {
  // Load all the settings.
  // @see pathauto_patterns_form in pathauto.admin.inc
  $all_settings = module_invoke_all('pathauto', 'settings');
  foreach ($all_settings as $settings) {
    // If the type is found, then set default pattern.
    if ($settings->token_type == $type) {
      $default_pattern = $settings->patterndefault;
      break;
    }
  }

  // Sends this back for evaluation.
  return isset($default_pattern) ? $default_pattern : NULL;
}

/**
 * Returns the overriden pattern for type indicated.
 *
 * @param  string $machine_name
 *   The machine name of the entity we are search for.
 *
 * @param  string $default_pattern
 *   The default pattern of the entity.
 *
 * @return string
 *   The overidden pattern for the type or the default pattern if none.
 */
function _kala_migrate_pathauto_overidden_pattern($machine_name, $default_pattern) {
  // @see pathauto_patterns_form in pathauto.admin.inc
  // Set the Variable Name to use.
  $variable = 'pathauto_node_' . $machine_name . '_pattern';

  // Return the appropriate pattern
  $pattern = variable_get($variable, '');
  return (empty($pattern)) ? $default_pattern : $pattern;
}

/**
 * Returns the field settings from the widget provided.
 *
 * @param  mixed $widget
 *   Mixed object of field settings from the widget.
 *
 * @return string
 *   The overidden pattern for the type or the default pattern if none.
 */
function _kala_migrate_get_field_settings($widget) {
  $settings = '';
  foreach ($widget as $key => $value) {
    // Convert bool to string.
    if (is_bool($value)) {
      $value = $value ? 'TRUE' : 'FALSE';
    }

    // Recursively flattens settings that are arrays.
    if (is_array($value)) {
      $flatvalue = _kala_migrate_get_field_settings($value);
      $value = rtrim($flatvalue, ' ,');
    }

    // Add brackets for better formatting
    $value = isset($flatvalue) ? '{' . $value . '}' : $value;

    // Throw the settings into a happu string.
    $settings .= '"' . $key . ' = ' . $value . '", ';
  }
  return $settings;
}
