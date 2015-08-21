<?php

/**
 * @file
 * Page callbacks for custom fun
 */

/*
 * Preprocess for theme_header_contact_block
 */
function template_preprocess_header_contact_block(&$vars) {
  $delta = $vars['opt']['delta'];
  $fields = $vars['opt']['fields'];
  foreach ($fields as $name => $field_set) {
    $var_key = "custom_fun_{$delta}_{$name}";

    $vars['build']["{$name}_wr"] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'custom-fan-' . $name . '-wr'
        )
      ),
    );
    if (isset($field_set['formatter'])) {
      switch ('mail_link') {
        case 'mail_link':
          $value = trim(variable_get($var_key, $field_set['default']));
          $vars['build']["{$name}_wr"][$name] = array(
            '#markup' => '<a href="mailto:' . $value . '">' . $value . '</a>',
          );
          break;
      }
    }
    else {
      $vars['build']["{$name}_wr"][$name] = array(
        '#markup' => variable_get($var_key, $field_set['default']),
      );
    }

  }
}

/*
 * Implements theme
 */

function theme_header_contact_block($vars) {
  return drupal_render($vars['build']);
}