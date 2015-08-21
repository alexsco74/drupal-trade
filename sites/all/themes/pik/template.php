<?php

/**
 * @file
 * Template overrides as well as (pre-)process and alter hooks for the pik
 * theme.
 */

/**
 * Check if a block is a menu block or not.
 *
 * @param stdClass $block
 *   A block object.
 *
 * @return bool
 *   Given block is a menu block.
 */
function _pik_is_menu_block($block) {
  $modules = array('menu', 'menu_block');
  if (in_array($block->module, $modules)) {
    return TRUE;
  }

  $modules = array('help', 'powered-by', 'main');
  if ($block->module == 'system' && !in_array($block->delta, $modules)) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Returns HTML for a breadcrumb trail.
 *
 * @param $variables
 *   An associative array containing:
 *   - breadcrumb: An array containing the breadcrumb links.
 */
function pik_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  if (($cur_title = drupal_get_title()) && !drupal_is_front_page()) {
    $breadcrumb[] = '<span>' . $cur_title . '</span>';
  }
  if (!empty($breadcrumb)) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<div class="element-invisible">' . t('You are here') . '</div>';

    $output .= '<div class="breadcrumb">' . implode('<span class="pik-bread-sep">></span>', $breadcrumb) . '</div>';
    return $output;
  }
}

function pik_image($variables) {
  $attributes = $variables['attributes'];
  $attributes['src'] = file_create_url($variables['path']);

  foreach (array('width', 'height', 'alt', 'title') as $key) {

    if (isset($variables[$key])) {
      $attributes[$key] = $variables[$key];
    }
  }

  //sas
  if (!empty($attributes['class'])
    && in_array('trade-img-data-src', $attributes['class'])
  ) {
    $image_loader_path = file_create_url(drupal_get_path('theme', 'pik') . '/images/image-loader.png');
    $attributes['data-src'] = $attributes['src'];
    $attributes['src'] = $image_loader_path;
  }
  //sas [end]

  return '<img' . drupal_attributes($attributes) . ' />';
}

/**
 * Implements hook_html_head_alter().
 */
function pik_html_head_alter(&$head) {

  //w3org validator fixed meta
  unset($head['system_meta_content_type']);
  unset($head['omega-cleartype']);
}