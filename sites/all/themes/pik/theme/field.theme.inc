<?php

/**
 * @file
 * Contains the theme function override for 'field'.
 */

/**
 * Returns HTML for a field.
 *
 * @ingroup themeable
 */
function pik_field($variables) {
  $output = '';
  $suffix = '';
  $dotted_label_fields = array('field_product_width', 'field_product_height', 'field_product_depth');


  if (in_array($variables['element']['#field_name'], $dotted_label_fields, TRUE)) {
    $suffix = ':&nbsp';
  }
  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<div class="field__label"' . $variables['title_attributes'] . '>' . $variables['label'] . $suffix . '</div>';
  }

  // Render the items.
  $output .= '<div class="field__items"' . $variables['content_attributes'] . '>';
  foreach ($variables['items'] as $delta => $item) {
    $classes = 'field__item ' . ($delta % 2 ? 'odd' : 'even');
    $output .= '<div class="' . $classes . '"' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</div>';
  }
  $output .= '</div>';

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . '"' . $variables['attributes'] . '>' . $output . '</div>';

  return $output;
}
