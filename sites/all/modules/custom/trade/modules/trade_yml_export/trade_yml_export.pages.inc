<?php

/**
 * @file
 * User page callbacks for the trade YML export module.
 */

function trade_yml_export_pages_get_xml() {
  $currency_code = variable_get('trade_currency', 'RUB');
  if (empty($currency_code) || $currency_code == 'RUB') {
    $currency_code = 'RUR';
  }
  $trade_yml_export_vid = trade_yml_export_get_catalog_vid();
  if (empty($trade_yml_export_vid)) {
    die('Please select primary vocabulary on YML export settings page!');
  }

  $trade_product_node_types = array_filter(variable_get('trade_product_node_types', array('product')));
  $ctypes = array_filter(variable_get('trade_yml_export_types', $trade_product_node_types));

  if (empty($ctypes)) {
    die('Please select at least one node type on YML export settings page!');
  }

  $categories = db_query("SELECT d.tid, d.name, h.* FROM {taxonomy_term_data} d LEFT JOIN {taxonomy_term_hierarchy} h USING(tid) WHERE d.vid=:vid", array(':vid' => $trade_yml_export_vid))->fetchAllAssoc('tid');

  $trade_catalog_field_name = variable_get('trade_product_catalog_field', 'field_product_catalog');
  $term_field = variable_get('trade_yml_export_term_field', $trade_catalog_field_name);
  $data = _trade_yml_export_get_nodes($ctypes, $term_field);

  $desc_field = variable_get('yml_export_descr_field', 'body');
  $trade_product_images_field_name = TRADE_FIELD_PRODUCT_IMAGES;
  $image_field = variable_get('trade_yml_export_image_field', $trade_product_images_field_name);

  header('Content-type: application/xhtml+xml; charset=utf-8');
  echo theme('trade_yml_products', array(
    'currency_code' => $currency_code,
    'nodes' => $data['nodes'],
    'nodes_tid' => $data['nodes_tid'],
    'categories' => $categories,
    'term_field' => $term_field,
    'desc_field' => $desc_field,
    'image_field' => $image_field,
  ));
  exit();
}