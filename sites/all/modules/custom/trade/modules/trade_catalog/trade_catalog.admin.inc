<?php

/**
 * @file
 * Administration page callbacks for the trade catalog module.
 */


function trade_catalog_admin_config_catalog_form() {
  $trade_product_node_types = array_filter(variable_get('trade_product_node_types', array()));
  $field_info_instances = array();

  foreach ($trade_product_node_types as $trade_product_node_type) {
    // get field instances
    $field_info_instances += _trade_catalog_get_field_instances('node', $trade_product_node_type, array(
      'taxonomy_term_reference'
    ));
  }

  //field_product_catalog
  if (!empty($field_info_instances)) {
    $form['trade_product_catalog_field'] = array(
      '#type' => 'select',
      '#title' => t('Product catalog field'),
      '#default_value' => variable_get('trade_product_catalog_field', 'field_product_catalog'),
      '#options' => $field_info_instances,
    );
  }

  //catalog node types
  if (!module_exists('trade')) {
    $node_type_opts = node_type_get_names();
    $form['trade_product_node_types'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Product node types'),
      '#options' => $node_type_opts,
      '#default_value' => variable_get('trade_product_node_types', array('product'))
    );
  }

  $product_bundles = array_filter(variable_get('trade_product_node_types', array('product')));

  if (module_exists('commerce') && !empty($product_bundles)) {
    $field_options = array();
    foreach ($product_bundles as $bundle) {
      $field_options += _trade_catalog_get_field_instances('node', $bundle, array('commerce_product_reference'));
    }
    $form['trade_catalog_commerce_product_field'] = array(
      '#type' => 'select',
      '#title' => t('Commerce product field'),
      '#options' => $field_options,
      '#default_value' => variable_get('trade_catalog_commerce_product_field', 'field_product_display_products')
    );
    $trade_catalog_commerce_product_field = variable_get('trade_catalog_commerce_product_field', 'field_product_display_products');
    if (!empty($trade_catalog_commerce_product_field)) {
      $field_options = array();
      foreach ($product_bundles as $bundle) {
        $field_options += _trade_catalog_get_field_instances('commerce_product', 'product', array('commerce_price'));
      }

      $form['trade_catalog_commerce_product_price_field'] = array(
        '#type' => 'select',
        '#title' => t('Commerce product price field'),
        '#options' => $field_options,
        '#default_value' => variable_get('trade_catalog_commerce_product_price_field', 'commerce_price')
      );
    }

  }

  //catalog fields
  $field_options = array();
  foreach ($product_bundles as $bundle) {
    $field_options += _trade_catalog_get_field_instances('node', $bundle, array(
      'taxonomy_term_reference',
      'number_integer',
      'number_decimal'
    ));
  }

  $form['trade_product_catalog_filter_fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Catalog filter fields'),
    '#options' => $field_options,
    '#default_value' => variable_get('trade_product_catalog_filter_fields', array())
  );

  $form['trade_product_catalog_page_count_nodes'] = array(
    '#type' => 'textfield',
    '#title' => t('Count nodes on page'),
    '#default_value' => variable_get('trade_product_catalog_page_count_nodes', 12),
    '#element_validate' => array('element_validate_number')
    // '#options' => drupal_map_assoc(array(0, 6, 12, 24, 48, 96))
  );

  //data-ajax-sel
  $form['trade_product_data_ajax_wrapper_selector'] = array(
    '#type' => 'textfield',
    '#title' => t('Product data ajax wrapper selector'),
    '#default_value' => variable_get('trade_product_data_ajax_wrapper_selector', ''),
    '#description' => t('Another selector for ajax load on sort link click')
  );

  // catalog tree term image style
  $image_styles = image_styles();
  $is_options = array();
  if (!empty($image_styles)) {
    foreach ($image_styles as $image_style) {
      $is_options[$image_style['name']] = $image_style['label'];
    }
  }

  //rewrite taxonomy term theme
  $form['trade_catalog_rewrite_taxonomy_term_theme'] = array(
    '#type' => 'checkbox',
    '#title' => t('Rewrite taxonomy term theme'),
    '#default_value' => variable_get('trade_catalog_rewrite_taxonomy_term_theme', FALSE)
  );

  $form['trade_product_catalog_page_terms_block_en'] = array(
    '#type' => 'checkbox',
    '#title' => t('Catalog page tree terms block enable'),
    '#default_value' => variable_get('trade_product_catalog_page_terms_block_en', FALSE),
  );

  $form['trade_product_catalog_select_product_current_term'] = array(
    '#type' => 'checkbox',
    '#title' => t('Catalog page select product only current term'),
    '#default_value' => variable_get('trade_product_catalog_select_product_current_term', FALSE),
  );

  //view modes
  if ($catalog_field = variable_get('trade_product_catalog_field', 'field_product_catalog')) {
    $field_info = field_info_field($catalog_field);
    if (!empty($field_info['settings']['allowed_values'][0]['vocabulary'])) {
      $view_modes_options = _trade_catalog_get_view_modes_custom_options('taxonomy_term', $field_info['settings']['allowed_values'][0]['vocabulary']);
      if(!empty($view_modes_options)) {
        $form['trade_product_catalog_tree_view_mode'] = array(
          '#type' => 'select',
          '#title' => t('Catalog tree term view mode'),
          '#default_value' => variable_get('trade_product_catalog_tree_view_mode', 'default'),
          '#options' => $view_modes_options,
        );
      }
    }
  }

  //enable masonry on catalog pages
  $form['trade_catalog_enable_masonry'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable masonry on catalog pages'),
    '#default_value' => variable_get('trade_catalog_enable_masonry', TRUE)
  );

  // $form['trade_catalog'] = drupal_get_form('trade_catalog_sort_fields_form');

  return system_settings_form($form);
}

function _trade_catalog_get_view_modes_custom_options($entity_type = 'taxonomy_term', $bundle = 'shop_category') {
  $options = array();
  $default = array();
  $entity_info = entity_get_info($entity_type);
  $view_modes = $entity_info['view modes'];
  $view_mode_settings = field_view_mode_settings($entity_type, $bundle);
  foreach ($view_modes as $view_mode_name => $view_mode_info) {
    if (!empty($view_mode_settings[$view_mode_name]['custom_settings'])) {
      $options[$view_mode_name] = $view_mode_info['label'];
    }
  }
  return $options;
}

function trade_catalog_sort_fields_form($form_state) {
  $form['filters']['#tree'] = TRUE;
  $sort_fields_elements = _trade_catalog_sort_fields_elements();
  if (!empty($sort_fields_elements)) {
    foreach ($sort_fields_elements as $sort_field) {
      $form['filters'][] = array(
        'name' => array(
          '#type' => 'hidden',
          '#prefix' => $sort_field['name'],
          '#value' => check_plain($sort_field['name']),
        ),
        'title' => array(
          '#type' => 'textfield',
          '#default_value' => check_plain($sort_field['title']),
          '#size' => 20,
          '#maxlength' => 255,
        ),
        'enable' => array(
          '#type' => 'checkbox',
          '#title' => '',
          '#default_value' => isset($sort_field['enable']) ? $sort_field['enable'] : FALSE,
        ),
        'default' => array(
          '#type' => 'checkbox',
          '#title' => '',
          '#default_value' => isset($sort_field['default']) ? $sort_field['default'] : FALSE,
        ),
        'direction' => array(
          '#type' => 'select',
          '#options' => array('asc' => 'Asc', 'desc' => 'Desc'),
          '#default_value' => isset($sort_field['direction']) ? $sort_field['direction'] : 'asc',
        ),
        'weight' => array(
          '#type' => 'weight',
          '#title' => t('Weight'),
          '#default_value' => $sort_field['weight'],
          '#delta' => 100,
          '#title_display' => 'invisible',
        ),
      );
    }
  }
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));
  return $form;
}

function theme_trade_catalog_sort_fields_form($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form['filters']) as $id) {

    $form['filters'][$id]['weight']['#attributes']['class'] = array('trade-catalog-filter-weight');
    $rows[] = array(
      'data' => array(
        // Add our 'name' column.
        drupal_render($form['filters'][$id]['name']),
        // Add our 'title' column.
        drupal_render($form['filters'][$id]['title']),
        // Add our 'enable' column.
        drupal_render($form['filters'][$id]['enable']),
        // Add our 'default' column.
        drupal_render($form['filters'][$id]['default']),
        // Add our 'direction' column.
        drupal_render($form['filters'][$id]['direction']),
        // Add our 'weight' column.
        drupal_render($form['filters'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }

  $header = array(t('Name'), t('title'), t('Enable'), t('Default'), t('Direction'), t('Weight'));
  $table_id = 'trade-catalog-filters-table';

  // We can render our tabledrag table for output.
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => $table_id),
  ));

  // And then render any remaining form elements (such as our submit button).
  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'trade-catalog-filter-weight');
  return $output;
}

function trade_catalog_sort_fields_form_submit($form, &$form_state) {
  $filters = array();
  foreach ($form_state['values']['filters'] as $id => $item) {
    $filters[$id] = array(
      'id' => $id,
      'name' => $item['name'],
      'title' => $item['title'],
      'enable' => $item['enable'],
      'default' => $item['default'],
      'direction' => $item['direction'],
      'weight' => $item['weight'],
    );
  }
  variable_del('trade_catalog_sort_fields');
  usort($filters, '_trade_catalog_filters_sort');
  variable_set('trade_catalog_sort_fields', $filters);
}