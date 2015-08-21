<?php

/**
 * @file
 * Contains a pre-process hook for 'page'.
 */

/**
 * Implements hook_preprocess_page().
 */
function pik_preprocess_page(&$variables, $hook) {
  global $theme_path;

  //fix front url
  $variables['front_page'] = url('<front>');
}