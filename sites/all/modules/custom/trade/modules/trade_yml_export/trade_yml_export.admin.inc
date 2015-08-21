<?php

/**
 * @file
 * Administration page callbacks for the trade yml export module.
 */


function trade_yml_export_admin_settings() {
  $form = array();
  $ctypes = array_intersect_key(node_type_get_names(), array_filter(variable_get('trade_product_node_types', array('product'))));

  $form['trade_yml_export_types'] = array(
    '#required' => TRUE,
    '#type' => 'checkboxes',
    '#options' => $ctypes,
    '#default_value' => variable_get('trade_yml_export_types', array('product' => 'product')),
    '#title' => t('Select node types for export'),
  );

  $instances = field_info_instances('node');
  if (variable_get('trade_yml_export_types', 0)) {
    $ctypes = variable_get('trade_yml_export_types', array('product' => 'product'));
    $ctype_first = key($ctypes); // we use first ctype to get available fields for now..
  }
  elseif (isset($ctypes['product'])) {
    $ctype_first = 'product';
  }
  else {
    current(array_filter($ctypes));
    $ctype_first = key($ctypes); // we use first ctype to get available fields for now..
  }

  $instance = $instances[$ctype_first];

  $term_fields = array();
  $descr_fields = array();

  foreach ($instance as $field_name => $field) {
    $info = field_info_field($field_name);

    if ($info['type'] == 'taxonomy_term_reference') {
      $term_fields[] = $field_name;
    }
    if ($info['module'] == 'text') {
      $descr_fields[] = $field_name;
    }
    if ($info['module'] == 'image') {
      $image_fields[] = $field_name;
    }
    if ($info['module'] == 'number') {
      $price_fields[] = $field_name;
    }

  }

  if (count($term_fields) == 0) {
    drupal_set_message(t("No term fields attached to product node! Export can't work properly. Please create at least one taxonomy vocabulary and use it for your products."), 'warning');
  }
  else {
    $term_fields = array_combine($term_fields, $term_fields);
  }

  if (count($descr_fields) == 0) {
    drupal_set_message(t("No text fields attached to product node! Export can't work properly. Please create at least one text field and use it for your products."), 'warning');
  }
  else {
    $descr_fields = array_combine($descr_fields, $descr_fields);
  }


  $trade_catalog_field_name = variable_get('trade_product_catalog_field', 'field_product_catalog');
  $form['trade_yml_export_term_field'] = array(
    '#required' => TRUE,
    '#type' => 'select',
    '#empty_value' => '',
    '#options' => $term_fields,
    '#default_value' => variable_get('trade_yml_export_term_field', $trade_catalog_field_name),
    '#description' => t('Select product term field where primary product categories are stored'),
    '#title' => t('Category field'),
  );

  $form['trade_yml_export_descr_field'] = array(
    '#required' => TRUE,
    '#type' => 'select',
    '#empty_value' => '',
    '#options' => $descr_fields,
    '#default_value' => variable_get('trade_yml_export_descr_field', 'body'),
    '#description' => t('Select text field which will be used as product description'),
    '#title' => t('Description field'),
  );

  $form['trade_yml_export_delivery'] = array(
    '#type' => 'select',
    '#options' => array('true' => t("true"), 'false' => t("false")),
    '#title' => t('Select if delivery is enabled'),
    '#default_value' => variable_get('trade_yml_export_delivery', 'true'),
    '#description' => t('Yandex.Market has "delivery" field. Select if it is enabled'),
  );

  if (empty($image_fields)) {
    drupal_set_message(t("No image fields attached to product node! Export can't work properly. Please create at least one image field and use it for your products."), 'warning');
  }
  else {
    $image_fields = array_combine($image_fields, $image_fields);
    $trade_product_images_field_name = TRADE_FIELD_PRODUCT_IMAGES;
    $form['trade_yml_export_image_field'] = array(
      '#required' => TRUE,
      '#type' => 'select',
      '#empty_value' => '',
      '#options' => $image_fields,
      '#default_value' => variable_get('trade_yml_export_image_field', $trade_product_images_field_name),
      '#description' => t('This field will be used for images in export.'),
      '#title' => t('Image field'),
    );
  }

  $url = url('yml_products.xml', array('absolute' => TRUE));

  $form['generated_url'] = array(
    '#type' => 'item',
    '#title' => t("Generated file"),
    '#markup' => $url . ' (' . l(t("View"), $url) . ')',
    '#description' => t("Use this url in Yandex.Market"),
  );

  return system_settings_form($form);
}