<?php
/**
 * @file
 * Implements the administrative configuration page for Go Dark For IE.
 */

/**
 * Settings form.
 */
function go_dark_for_ie_settings($form, &$form_state) {
  $options = array('query' => array('go_dark_for_ie' => 1));

  $form['go_dark_for_ie_enable'] = array(
    '#title' => t('Go dark for old versions of IE'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('go_dark_for_ie_enable', TRUE),
    '#description' => t('Darkens website on October 26, 2012 for all users of IE8 and older.') . ' ' .
      l(t('Click here to preview the blackout page.'), '<front>', $options),
  );

  return system_settings_form($form);
}
