<?php

/**
 * @file
 * Cart page callbacks for the trade order module.
 */

function trade_order_page_edit($order) {
  global $language;
  $title = t('Edit order #@id', array('@id' => $order->order_id));
  drupal_set_title($title);

  $build = array(
    '#theme' => 'trade_order_edit',
    '#order' => $order,
  );
  return $build;
}

function trade_order_service_form($form, &$form_state, $data = array(), $service_name = 'shipping') {
  $form = $data;
  return $form;
}

/*
 * build products
 */
function trade_order_build_services($order = NULL, $build_products = array(), $mode = 'full', $view_mode = 'full') {
  global $language;
  $build = array();
  $rows = array();
  $total = array();
  $currency = _trade_get_currency($order->currency);
  $product_services = _trade_get_products_services($order);

  if (!empty($order->service)) {
    $order_service = unserialize($order->service);
  }
  else {
    $order_service = array();
  }

  $service_type_options['shipping'] = array(
    'fix_amount' => t('On city (amount)'),
    'price_on_distance' => t('From city'),
  );

  //custom shipping
  if (db_table_exists('trade_shipping')) {
    $custom_shipping_objs = trade_shipping_load_multiply();
    foreach ($custom_shipping_objs as $custom_shipping_key => $custom_shipping_obj) {
      $element_key = 'custom_' . $custom_shipping_obj->sid;
      $service_type_options['shipping'][$element_key] = $custom_shipping_obj->title;
    }
  }

  $service_type_options['lifting'] = array(
    'lift' => t('On lift'),
    'steps' => t('Without lift'),
  );
  $service_type_options['assembly'] = array(
    'assembly' => t('Assembly'),
  );

  $service_type_empty_options['shipping'] = array(
    '_none' => t('Without shipping'),
  );
  $service_type_empty_options['lifting'] = array(
    '_none' => t('Without lifting'),
  );
  $service_type_empty_options['assembly'] = array(
    '_none' => t('Without assembly'),
  );

  foreach ($service_type_options as $service_type_options_name => $service_type_options_set) {
    $data = array();
    $shipping_amount_element = array();
    switch ($service_type_options_name) {

      case 'shipping':

        // shipping
        if (isset($product_services[$service_type_options_name])) {
          $service_type_options_set = array_intersect_key($service_type_options_set, $product_services[$service_type_options_name]);

          if (count($service_type_options_set) > 0) {
            $service_type_options_set = $service_type_empty_options[$service_type_options_name] + $service_type_options_set;

            if (!empty($order_service[$service_type_options_name]['type'])) {
              $service_inner_type = $order_service[$service_type_options_name]['type'];
            }
            else {
              $service_inner_type = key($service_type_options_set);
            }

            if ($mode == 'full') {
              $data[$service_type_options_name]['shipping_type_item'] = array(
                '#type' => 'container',
                '#attributes' => array('class' => array('trade-form-item')),
                'title' => array(
                  '#theme' => 'html_tag',
                  '#tag' => 'label',
                  '#attributes' => array('class' => array('trade-field-title')),
                  '#value' => t('Type') . ':',
                ),
                'shipping_type' => array(
                  '#type' => 'select',
                  '#default_value' => $service_inner_type,
                  '#options' => $service_type_options_set,
                  '#attributes' => array(
                    'id' => drupal_html_id('edit-service-shipping-type-' . microtime(TRUE)),
                    'data-def-value' => $service_inner_type,
                    'data-field-name' => 'type',
                    'data-order-line-id' => $service_type_options_name,
                    'data-order-line-type' => 'service',
                    'data-order-line-operation' => 'edit',
                  )
                ),
              );
            }
            else {
              $shipping_type_title = '';
              if (isset($service_type_options_set[$service_inner_type])) {
                $shipping_type_title = $service_type_options_set[$service_inner_type];
              }
              if ($shipping_type_title) {
                $data[$service_type_options_name]['shipping_type'] = array(
                  'shipping_type_item' => array(
                    '#type' => 'container',
                    '#attributes' => array('class' => array('trade-form-item')),
                    'title' => array(
                      '#theme' => 'html_tag',
                      '#tag' => 'label',
                      '#attributes' => array('class' => array('trade-field-title')),
                      '#value' => t('Type') . ':',
                    ),
                    'shipping_type' => array('#markup' => $shipping_type_title),
                  ),
                );
              }
            }


            //default values
            $default_values = array();
            $default_field_names = array('amount', 'price', 'distance');
            foreach ($default_field_names as $default_field_name) {
              if (!empty($order_service[$service_type_options_name][$default_field_name])) {
                $default_values[$default_field_name] = $order_service[$service_type_options_name][$default_field_name];
              }
              elseif (!empty($product_services[$service_type_options_name][$service_inner_type][$default_field_name])) {
                $default_values[$default_field_name] = $product_services[$service_type_options_name][$service_inner_type][$default_field_name];
              }
              else {
                $default_values[$default_field_name] = 0;
              }
            }

            switch ($service_inner_type) {
              case '_none':
                break;
              case 'fix_amount':
                $shipping_amount_element = array(
                  '#markup' => '<span>' . _trade_format_number($default_values['amount']) . '</span>',
                );
                break;
              case 'price_on_distance':
                // price
                $data[$service_type_options_name]['shipping_price_item'] = array(
                  '#type' => 'container',
                  '#attributes' => array('class' => array('trade-form-item')),
                  'title' => array(
                    '#theme' => 'html_tag',
                    '#tag' => 'label',
                    '#attributes' => array('class' => array('trade-field-title')),
                    '#value' => t('Price') . ':',
                  ),
                  'shipping_price' => array(
                    '#type' => 'item',
                    '#markup' => '<span>' . _trade_format_number($default_values['price']) . '</span>',
                  ),
                );

                if ($view_mode == 'mail') {
                  unset($data[$service_type_options_name]['shipping_price_item']['shipping_price']['#type']);
                }

                if ($mode == 'full') {
                  $data[$service_type_options_name]['shipping_distance_item'] = array(
                    '#type' => 'container',
                    '#attributes' => array('class' => array('trade-form-item')),
                    'title' => array(
                      '#theme' => 'html_tag',
                      '#tag' => 'label',
                      '#attributes' => array('class' => array('trade-field-title')),
                      '#value' => t('Distance (km)') . ':',
                    ),
                    'shipping_distance' => array(
                      '#type' => 'textfield',
                      '#title_display' => 'before',
                      '#size' => 7,
                      '#maxlength' => 5,
                      '#default_value' => $default_values['distance'],
                      '#attributes' => array(
                        'id' => drupal_html_id('edit-service-shipping-type-' . microtime(TRUE)),
                        'class' => array('trade-spinner'),
                        'data-def-value' => $service_inner_type,
                        'data-field-name' => 'distance',
                        'data-order-line-id' => $service_type_options_name,
                        'data-order-line-type' => 'service',
                        'data-order-line-operation' => 'edit',
                      )
                    ),
                  );
                }
                else {
                  $data[$service_type_options_name]['shipping_distance_item'] = array(
                    '#type' => 'container',
                    '#attributes' => array('class' => array('trade-form-item')),
                    'title' => array(
                      '#theme' => 'html_tag',
                      '#tag' => 'label',
                      '#attributes' => array('class' => array('trade-field-title')),
                      '#value' => t('Distance (km)') . ':',
                    ),
                    'shipping_distance' => array(
                      '#markup' => _trade_format_number($default_values['distance']),
                    ),
                  );
                }

                $shipping_amount_element = array(
                  '#markup' => '<span>' . _trade_format_number($default_values['amount']) . '</span>',
                );

                break;
              default:
                if (strpos($service_inner_type, 'custom_') === 0) {
                  $shipping_amount_element = array(
                    '#markup' => '<span>' . _trade_format_number($default_values['amount']) . '</span>',
                  );
                }
                break;
            }
            if (empty($shipping_amount_element)) {
              $shipping_amount_element = array(
                '#markup' => '<span>0</span>',
              );
            }
            $render_var = drupal_get_form('trade_order_service_form', $data[$service_type_options_name], $service_type_options_name);
            $row = array(
              array(
                'class' => 'trade-service-title',
                'style' => 'width:100px;',
                'data' => t('Shipping')
              ),
              array(
                'class' => 'trade-service-calc',
                'data' => drupal_render($render_var),
              ),
              array(
                'style' => 'width:120px;' . ($view_mode == 'mail' ? 'text-align:right;' : ''),
                'class' => array('trade-format-number'),
                'data' => drupal_render($shipping_amount_element)
              ),
            );

            if (isset($total['amount'])) {
              $total['amount'] += $default_values['amount'];
            }
            else {
              $total['amount'] = $default_values['amount'];
            }
            $rows[] = $row;
          }
        }
        break;

      case 'lifting':

        // lifting
        if (isset($product_services[$service_type_options_name])) {

          $service_type_options_set = array_intersect_key($service_type_options_set, $product_services[$service_type_options_name]);

          if (count($service_type_options_set) > 0) {
            $service_type_options_set = $service_type_empty_options[$service_type_options_name] + $service_type_options_set;

            if (!empty($order_service[$service_type_options_name]['type'])) {
              $service_inner_type = $order_service[$service_type_options_name]['type'];
            }
            else {
              $service_inner_type = key($service_type_options_set);
            }

            if ($mode == 'full') {
              $data[$service_type_options_name][$service_type_options_name . '_type_item'] = array(
                '#type' => 'container',
                '#attributes' => array('class' => array('trade-form-item')),
                'title' => array(
                  '#theme' => 'html_tag',
                  '#tag' => 'label',
                  '#attributes' => array('class' => array('trade-field-title')),
                  '#value' => t('Type') . ':',
                ),
                $service_type_options_name . '_type' => array(
                  '#type' => 'select',
                  '#default_value' => $service_inner_type,
                  '#options' => $service_type_options_set,
                  '#attributes' => array(
                    'id' => drupal_html_id('edit-service-' . $service_type_options_name . '-type-' . microtime(TRUE)),
                    'data-def-value' => $service_inner_type,
                    'data-field-name' => 'type',
                    'data-order-line-id' => $service_type_options_name,
                    'data-order-line-type' => 'service',
                    'data-order-line-operation' => 'edit',
                  )
                ),
              );
            }
            else {
              $data[$service_type_options_name][$service_type_options_name . '_type'] = array(
                $service_type_options_name . '_type_item' => array(
                  '#type' => 'container',
                  '#attributes' => array('class' => array('trade-form-item')),
                  'title' => array(
                    '#theme' => 'html_tag',
                    '#tag' => 'label',
                    '#attributes' => array('class' => array('trade-field-title')),
                    '#value' => t('Type') . ':',
                  ),
                  $service_type_options_name . '_type' => array('#markup' => $service_type_options_set[$service_inner_type]),
                ),
              );
            }

            //default values
            $default_values = array();
            $default_field_names = array('amount', 'price', 'distance', 'floor');
            foreach ($default_field_names as $default_field_name) {
              if (!empty($order_service[$service_type_options_name][$default_field_name])) {
                $default_values[$default_field_name] = $order_service[$service_type_options_name][$default_field_name];
              }
              elseif (!empty($product_services[$service_type_options_name][$service_inner_type][$default_field_name])) {
                $default_values[$default_field_name] = $product_services[$service_type_options_name][$service_inner_type][$default_field_name];
              }
              else {
                $default_values[$default_field_name] = 0;
              }
            }

            switch ($service_inner_type) {
              case '_none':
                break;
              case 'lift':
                $shipping_amount_element = array(
                  '#markup' => '<span>' . _trade_format_number($default_values['amount']) . '</span>',
                );
                break;
              case 'steps':
                // price
                $data[$service_type_options_name][$service_type_options_name . '_price_item'] = array(
                  '#type' => 'container',
                  '#attributes' => array('class' => array('trade-form-item')),
                  'title' => array(
                    '#theme' => 'html_tag',
                    '#tag' => 'label',
                    '#attributes' => array('class' => array('trade-field-title')),
                    '#value' => t('Price') . ':',
                  ),
                  $service_type_options_name . '_price' => array(
                    '#type' => 'item',
                    '#markup' => '<span>' . _trade_format_number($default_values['price']) . '</span>',
                  ),
                );

                if ($view_mode == 'mail') {
                  unset($data[$service_type_options_name][$service_type_options_name . '_price_item'][$service_type_options_name . '_price']['#type']);
                }

                if ($mode == 'full') {
                  $data[$service_type_options_name][$service_type_options_name . '_distance_item'] = array(
                    '#type' => 'container',
                    '#attributes' => array('class' => array('trade-form-item')),
                    'title' => array(
                      '#theme' => 'html_tag',
                      '#tag' => 'label',
                      '#attributes' => array('class' => array('trade-field-title')),
                      '#value' => t('Floor') . ':',
                    ),
                    $service_type_options_name . '_floor' => array(
                      '#type' => 'textfield',
                      '#title_display' => 'before',
                      '#size' => 7,
                      '#maxlength' => 5,
                      '#default_value' => $default_values['floor'],
                      '#attributes' => array(
                        'id' => drupal_html_id('edit-service-' . $service_type_options_name . '-type-' . microtime(TRUE)),
                        'class' => array('trade-spinner'),
                        'data-def-value' => $service_inner_type,
                        'data-field-name' => 'floor',
                        'data-order-line-id' => $service_type_options_name,
                        'data-order-line-type' => 'service',
                        'data-order-line-operation' => 'edit',
                      )
                    ),
                  );
                }
                else {
                  $data[$service_type_options_name][$service_type_options_name . '_floor_item'] = array(
                    '#type' => 'container',
                    '#attributes' => array('class' => array('trade-form-item')),
                    'title' => array(
                      '#theme' => 'html_tag',
                      '#tag' => 'label',
                      '#attributes' => array('class' => array('trade-field-title')),
                      '#value' => t('Floor') . ':',
                    ),
                    $service_type_options_name . '_floor' => array(
                      '#markup' => _trade_format_number($default_values['floor']),
                    ),
                  );
                }

                $shipping_amount_element = array(
                  '#markup' => '<span>' . _trade_format_number($default_values['amount']) . '</span>',
                );

                break;
            }
            if (empty($shipping_amount_element)) {
              $shipping_amount_element = array(
                '#markup' => '<span>0</span>',
              );
            }
            $row = array(
              array(
                'class' => 'trade-service-title',
                'style' => 'width:100px;',
                'data' => t('Lifting')
              ),
              array(
                'class' => 'trade-service-calc',
                'data' => drupal_render(drupal_get_form('trade_order_service_form', $data[$service_type_options_name], $service_type_options_name))
              ),
              array(
                'style' => 'width:120px;' . ($view_mode == 'mail' ? 'text-align:right;' : ''),
                'class' => array('trade-format-number'),
                'data' => drupal_render($shipping_amount_element)
              ),
            );

            if (isset($total['amount'])) {
              $total['amount'] += $default_values['amount'];
            }
            else {
              $total['amount'] = $default_values['amount'];
            }
            $rows[] = $row;
          }
        }
        break;
      case 'assembly':

        // assembly
        if (isset($product_services[$service_type_options_name])) {

          $service_type_options_set = array_intersect_key($service_type_options_set, $product_services[$service_type_options_name]);

          if (count($service_type_options_set) > 0) {
            $service_type_options_set = $service_type_empty_options[$service_type_options_name] + $service_type_options_set;

            if (!empty($order_service[$service_type_options_name]['type'])) {
              $service_inner_type = $order_service[$service_type_options_name]['type'];
            }
            else {
              $service_inner_type = key($service_type_options_set);
            }

            if ($mode == 'full') {
              $data[$service_type_options_name][$service_type_options_name . '_type_item'] = array(
                '#type' => 'container',
                '#attributes' => array('class' => array('trade-form-item')),
                'title' => array(
                  '#theme' => 'html_tag',
                  '#tag' => 'label',
                  '#attributes' => array('class' => array('trade-field-title')),
                  '#value' => t('Type') . ':',
                ),
                $service_type_options_name . '_type' => array(
                  '#type' => 'select',
                  '#default_value' => $service_inner_type,
                  '#options' => $service_type_options_set,
                  '#attributes' => array(
                    'id' => drupal_html_id('edit-service-' . $service_type_options_name . '-type-' . microtime(TRUE)),
                    'data-def-value' => $service_inner_type,
                    'data-field-name' => 'type',
                    'data-order-line-id' => $service_type_options_name,
                    'data-order-line-type' => 'service',
                    'data-order-line-operation' => 'edit',
                  )
                ),
              );
            }
            else {
              $data[$service_type_options_name][$service_type_options_name . '_type'] = array(
                $service_type_options_name . '_type_item' => array(
                  '#type' => 'container',
                  '#attributes' => array('class' => array('trade-form-item')),
                  'title' => array(
                    '#theme' => 'html_tag',
                    '#tag' => 'label',
                    '#attributes' => array('class' => array('trade-field-title')),
                    '#value' => t('Type') . ':',
                  ),
                  $service_type_options_name . '_type' => array('#markup' => $service_type_options_set[$service_inner_type]),
                ),
              );
            }

            //default values
            $default_values = array();
            $default_field_names = array('amount', 'price', 'distance', 'floor');
            foreach ($default_field_names as $default_field_name) {
              if (!empty($order_service[$service_type_options_name][$default_field_name])) {
                $default_values[$default_field_name] = $order_service[$service_type_options_name][$default_field_name];
              }
              elseif (!empty($product_services[$service_type_options_name][$service_inner_type][$default_field_name])) {
                $default_values[$default_field_name] = $product_services[$service_type_options_name][$service_inner_type][$default_field_name];
              }
              else {
                $default_values[$default_field_name] = 0;
              }
            }

            switch ($service_inner_type) {
              case '_none':
                break;
              case 'assembly':
                $shipping_amount_element = array(
                  '#markup' => '<span>' . _trade_format_number($default_values['amount']) . '</span>',
                );
                break;
            }
            if (empty($shipping_amount_element)) {
              $shipping_amount_element = array(
                '#markup' => '<span>0</span>',
              );
            }
            $row = array(
              array(
                'class' => 'trade-service-title',
                'style' => 'width:100px;',
                'data' => t('Assembly')
              ),
              array(
                'class' => 'trade-service-calc',
                'data' => drupal_render(drupal_get_form('trade_order_service_form', $data[$service_type_options_name], $service_type_options_name))
              ),
              array(
                'style' => 'width:120px;' . ($view_mode == 'mail' ? 'text-align:right;' : ''),
                'class' => array('trade-format-number'),
                'data' => drupal_render($shipping_amount_element)
              ),
            );

            if (isset($total['amount'])) {
              $total['amount'] += $default_values['amount'];
            }
            else {
              $total['amount'] = $default_values['amount'];
            }
            $rows[] = $row;
          }
        }
        break;
    }
  }

  if (!empty($rows)) {
    $caption = t('Services');
    $total_title = t('Total');

    if (!empty($total['amount'])) {
      $rows['total'] = array(
        'class' => array('trade-order-product-data-total'),
        'data' => array(
          array('data' => $total_title, 'colspan' => '2'),
          array(
            'data' => _trade_format_number($total['amount']),
            'class' => array('trade-format-number'),
            'style' => $view_mode == 'mail' ? 'text-align:right;' : ''
          ),
        )
      );
    }
    $header = array(
      array('data' => t('Service name')),
      array('data' => '&nbsp;'),
      array(
        'data' => t('Amount') . " ({$currency[1]})",
        'class' => array('trade-format-number')
      ),
    );
    $build = array(
      '#prefix' => '<h3>' . $caption . '</h3>',
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#sticky' => FALSE,
      '#attributes' => array('class' => array('trade-order-service-data')),
    );

    if ($view_mode == 'mail') {
      $build['#attributes']['border'] = '1';
      $build['#attributes']['cellpadding'] = '5';
      $build['#attributes']['width'] = '100%';
    }
  }
  return $build;
}

/*
 * build products
 */
function trade_order_build_products($order = NULL, $mode = 'full', $view_mode = 'full') {
  global $user, $language;

  $build = array();
  $currency = _trade_get_currency($order->currency);
  $rows = array();
  $total = array();

  $header = array(
    '#',
    t('SKU'),
    t('Product name'),
    array('data' => t('Price'), 'class' => array('trade-format-number')),
    array(
      'data' => t('Qty'),
      'class' => array('trade-format-number')
    ),
    array(
      'data' => t('Amount') . " ({$currency[1]})",
      'class' => array('trade-format-number')
    ),

  );

  if ($mode == 'full'
    && (empty($order->status)
      || (_trade_order_is_my($order) && user_access('delete own orders'))
      || user_access('delete all orders') || $user->uid == 1)
  ) {
    $header[] = t('Delete');
  }

  // product
  $i = 0;
  $quantity_form_item = array();
  $delete_form_item = array();

  if (!empty($order->lines)) {
    foreach ($order->lines as $line) {
      $i++;
      $options = array();

      // quantity element
      if ($mode == 'full' && ((_trade_order_is_my($order) && (empty($order->status) || user_access('edit own orders'))) || user_access('edit all orders') || $user->uid == 1)
      ) {
        $render_var = drupal_get_form('trade_order_quantity_form_' . $line->order_line_id, $line->quantity, $line->order_line_id);
        $quantity_form_item[$line->order_line_id] = drupal_render($render_var);
      }
      else {
        $quantity_form_item[$line->order_line_id] = _trade_format_number($line->quantity);
      }

      // delete element
      if ($mode == 'full'
        && (empty($order->status)
          || (_trade_order_is_my($order) && user_access('delete own orders'))
          || user_access('delete all orders') || $user->uid == 1)
      ) {
        $render_var = drupal_get_form('trade_order_delete_form_' . $line->order_line_id, $line->order_line_id);
        $delete_form_item[$line->order_line_id] = drupal_render($render_var);
      }
      // options
      if (!empty($line->options)) {
        foreach ($line->options as $op_keys => $option) {
          $options[] = theme('trade_option_view', array(
            'entity_id' => $line->order_line_id,
            'entity_bundle' => $line->type,
            'option' => $option
          ));
        }
      }
      else {
        $options = array();
      }

      $sku = "&nbsp";
      $line_order_entity = node_load($line->order_entity_id);
      if (!empty($line_order_entity)) {
        $sku_items = field_get_items('node', $line_order_entity, 'field_product_code');
        if (!empty($sku_items[0]['value'])) {
          $sku = $sku_items[0]['value'];
        }
      }
      $row = array(
        $i,
        $sku,
        $line->title . theme('item_list', array(
          'items' => $options,
          'attributes' => array('class' => 'trade-options-view')
        )),
        'price' => array(
          'data' => _trade_format_number($line->price),
          'class' => array('trade-format-number'),
          'style' => $view_mode == 'mail' ? 'text-align:right;' : ''
        ),
        'quantity' => array(
          'data' => $quantity_form_item[$line->order_line_id],
          'class' => array('trade-format-number'),
          'style' => $view_mode == 'mail' ? 'text-align:right;' : ''
        ),
        'amount' => array(
          'data' => _trade_format_number($line->amount),
          'class' => array('trade-format-number'),
          'style' => $view_mode == 'mail' ? 'text-align:right;' : ''
        ),
      );

      if ($mode == 'full'
        && (empty($order->status)
          || (_trade_order_is_my($order) && user_access('delete own orders'))
          || user_access('delete all orders') || $user->uid == 1)
      ) {
        $row[] = $delete_form_item[$line->order_line_id];
      }

      $rows[] = $row;
      if (isset($total['quantity'])) {
        $total['quantity'] += $line->quantity;
      }
      else {
        $total['quantity'] = $line->quantity;
      }
      if (isset($total['amount'])) {
        $total['amount'] += $line->amount;
      }
      else {
        $total['amount'] = $line->amount;
      }

    }
  }

  if (!empty($rows)) {

    if ($mode == 'full'
      && (empty($order->status)
        || (_trade_order_is_my($order) && user_access('delete own orders'))
        || user_access('delete all orders') || $user->uid == 1)
    ) {
      $col_span = 3;
    }
    else {
      $col_span = 2;
    }

    $total_title = t('Total');
    $rows['total'] = array(
      'class' => array('trade-order-product-data-total'),
      'data' => array(
        array('data' => $total_title, 'colspan' => count($header) - $col_span),
        array(
          'data' => _trade_format_number($total['quantity']),
          'class' => array('trade-format-number'),
          'style' => $view_mode == 'mail' ? 'text-align:right;' : ''
        ),
        array(
          'data' => _trade_format_number($total['amount']),
          'class' => array('trade-format-number'),
          'style' => $view_mode == 'mail' ? 'text-align:right;' : ''
        ),
      )
    );
    if ($mode == 'full'
      && (empty($order->status)
        || (_trade_order_is_my($order) && user_access('delete own orders'))
        || user_access('delete all orders') || $user->uid == 1)
    ) {
      $rows['total']['data'][] = "&nbsp;";
    }
  }

  $caption = t('Products');
  $build = array(
    '#prefix' => '<h3>' . $caption . '</h3>',
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#sticky' => FALSE,
    '#attributes' => array('class' => array('trade-order-product-data')),
    '#empty' => t('Product not choiced'),
  );

  if ($view_mode == 'mail') {
    $build['#attributes']['border'] = '1';
    $build['#attributes']['cellpadding'] = '5';
    $build['#attributes']['width'] = '100%';
  }

  return $build;
}

function template_preprocess_trade_order_edit(&$vars) {
  global $language;

  $m_path = drupal_get_path('module', 'trade');
  drupal_add_css($m_path . '/css/trade_order.css');
  drupal_add_js($m_path . '/js/trade_order.js');

  $order = $vars['order'];

  $vars['build'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'trade-order-page',
        'trade-order-container',
        'trade-order-edit',
      ),
      'data-order-id' => $order->order_id,
    ),
  );

  if (!empty($vars['options']['hidden'])) {
    $vars['build']['#attributes']['class'][] = 'trade-hidden';
  }

  $build = array();

  // build products
  $build['products'] = trade_order_build_products($order);

  if (!empty($order->status)) {
    $vars['build']['date_created'] = array(
      '#type' => 'item',
      '#title' => t('Date created') . ':',
      'field_item' => array(
        '#theme' => 'html_tag',
        '#tag' => 'span',
        '#value' => format_date(strtotime($order->created), 'short'),
      )
    );
  }

// build services
  $build['services'] = trade_order_build_services($order, $build['products']);

  if ($order->status || !empty($build['products']['#rows'])) {
    $vars['build']['products'] = $build['products'];
  }

  if ($order->status || !empty($build['products']['#rows']) && !empty($build['services'])) {
    $vars['build']['services'] = $build['services'];
  }

  if (!empty($order->amount)) {
    $currency = _trade_get_currency($order->currency);
    $vars['build']['line'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'hr',
    );
    $vars['build']['amount_item'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('trade-order-total-all'),
      ),
      'amount' => array(
        '#type' => 'item',
        '#title' => t('Total amount') . ':',
        'field_item' => array(
          '#theme' => 'html_tag',
          '#tag' => 'span',
          '#value' => _trade_format_number($order->amount) . ' ' . $currency[1],
        ),
      ),
    );
  }

  if (!empty($order->status) || !empty($build['products']['#rows'])) {
    $vars['build']['order'] = drupal_get_form('trade_order_form', $order);
  }

  if (empty($order->status) && empty($build['products']['#rows'])) {

    //empty cart
    $vars['build']['empty'] = array(
      '#markup' => t('You cart is empty. You can !choice product.', array('!choice' => l(t('choice'), '<front>')))
    );
  }
}

function theme_trade_order_edit($vars) {
  return drupal_render($vars['build']);
}

function trade_order_form($form, &$form_state, $order = NULL) {
  global $user, $language, $base_url;

  if (strpos(request_path(), '/action-edit') && isset($_SERVER['HTTP_REFERER'])) {
    $form['#action'] = str_replace($base_url, '', $_SERVER['HTTP_REFERER']);
  }

  if (empty($order->status)) {
    $default_values = _trade_order_get_default_values();
  }

  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );
  $form['order_data_table'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('trade-table', 'trade-order-data'))
  );
  $form['order_data_table']['row_one'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('trade-table-row'))
  );
  $form['order_data_table']['row_one']['cell_one'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('trade-table-cell', 'first'))
  );
  $title = t('Full name');
  $form['order_data_table']['row_one']['cell_one']['name'] = array(
    '#type' => 'textfield',
    '#title' => $title,
    '#title_display' => 'invisible',
    '#size' => 30,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => $title,
      'required' => 'required',
    ),
    '#default_value' => !empty($order->name) ? $order->name : (!empty($default_values->name) ? $default_values->name : '')
  );
  $form['order_data_table']['row_one']['cell_one']['phone'] = array(
    '#type' => 'textfield',
    '#attributes' => array(
      'placeholder' => t('Phone'),
    ),
    '#size' => 30,
    '#default_value' => !empty($order->phone) ? $order->phone : (!empty($default_values->phone) ? $default_values->phone : '')
  );
  $title = t('E-mail');
  $form['order_data_table']['row_one']['cell_one']['mail'] = array(
    '#type' => 'textfield',
    '#title' => $title,
    '#title_display' => 'invisible',
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => $title,
      'required' => 'required',
    ),
    '#size' => 30,
    '#default_value' => !empty($order->mail) ? $order->mail : (!empty($default_values->mail) ? $default_values->mail : '')
  );
  $form['order_data_table']['row_one']['cell_two'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('trade-table-cell'))
  );
  $form['order_data_table']['row_one']['cell_two']['address'] = array(
    '#type' => 'textarea',
    '#resizable' => FALSE,
    '#attributes' => array(
      'placeholder' => t('Address of house'),
    ),
    '#rows' => 3,
    '#default_value' => !empty($order->address) ? $order->address : (!empty($default_values->address) ? $default_values->address : '')
  );
  $form['order_data_table']['row_one']['cell_three'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('trade-table-cell', 'last'))
  );
  $form['order_data_table']['row_one']['cell_three']['comment'] = array(
    '#type' => 'textarea',
    '#resizable' => FALSE,
    '#attributes' => array(
      'placeholder' => t('Address details'),
    ),
    '#rows' => 3,
    '#default_value' => !empty($order->comment) ? $order->comment : (!empty($default_values->comment) ? $default_values->comment : '')
  );

  if (empty($order->status)) {
    $submit_value = t('Make order');
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => $submit_value,
    );

  }
  else {
    $submit_value = t('Save');
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => $submit_value,
    );
    if ((_trade_order_is_my($order) && user_access('delete own orders'))
      || user_access('delete all orders')
      || $user->uid == 1
    ) {
      $delete_value = t('Delete');
      $form['delete'] = array(
        '#type' => 'submit',
        '#value' => $delete_value,
      );
    }
  }

  return $form;
}

function trade_order_delete_form($form, &$form_state, $order_line_id = 0) {
  global $language;

  $form['order_line_delete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete'),
    '#default_value' => 0,
    '#attributes' => array(
      //'id' => drupal_html_id('edit-delete-' . microtime(TRUE)),
      'class' => array('trade-order-line-delete'),
      'data-field-name' => 'order_line_delete',
      'data-order-line-id' => $order_line_id,
      'data-order-line-type' => 'product',
      'data-order-line-operation' => 'delete',
    ),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'trade') . '/css/trade_spinner.css'),
      'js' => array(drupal_get_path('module', 'trade') . '/js/trade_spinner.js'),
    ),
  );
  return $form;
}

function trade_order_quantity_form($form, &$form_state, $quantity = 0, $order_line_id = 0) {
  $default_value = _trade_format_number($quantity);
  $form['quantity'] = array(
    '#type' => 'textfield',
    '#size' => 2,
    '#maxlength' => 2,
    '#default_value' => _trade_format_number($quantity),
    '#attributes' => array(
      'id' => drupal_html_id('edit-quantity-' . microtime(TRUE)),
      'class' => array('trade-spinner'),
      'data-def-value' => $default_value,
      'data-field-name' => 'quantity',
      'data-order-line-id' => $order_line_id,
      'data-order-line-type' => 'product',
      'data-order-line-operation' => 'edit',
    ),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'trade') . '/css/trade_spinner.css'),
      'js' => array(drupal_get_path('module', 'trade') . '/js/trade_spinner.js'),
    ),
  );
  return $form;
}

function trade_order_form_validate($form, &$form_state) {
  drupal_get_messages();
  // Validate the e-mail address, and check if it is taken by an existing user.
  if ($error = user_validate_mail($form_state['values']['mail'])) {
    form_set_error('mail', '');
  }
}

function trade_order_form_submit($form, &$form_state) {
  global $language;

  $values = $form_state['values'];
  $order = array();
  $message = '';
  $invoke_op = '';
  $op = array();
  $order_keys = array('order_id', 'name', 'phone', 'mail', 'address', 'comment');
  foreach ($values as $key => $value) {
    $value = trim(filter_xss($value, array()));
    $order[$key] = $value;
  }
  if (!empty($order['order_id'])) {
    $op['order'] = t('Make order');
    $op['save'] = t('Save');
    $op['delete'] = t('Delete');

    $date_iso = format_date(REQUEST_TIME, 'custom', 'c');

    if ($values['op'] == $op['order']) {
      $order['status'] = 1;
      $order['created'] = $order['changed'] = $date_iso;
      $invoke_op = 'create';
      $order_obj = (object) $order;
      $message = t('Thanks, order is complete.');
      $form_state['redirect'] = 'trade/orders';
    }
    elseif ($values['op'] == $op['save']) {
      $order['changed'] = $date_iso;
      $invoke_op = 'update';
      $order_before = trade_order_load($order['order_id']);
      if (!empty($order_before)) {
        $order['created'] = $order_before->created;
      }
      $order_obj = (object) $order;
      $message = t('Order saved.');
    }
    elseif ($values['op'] == $op['delete']) {
      $order_before = trade_order_load($order['order_id']);
      if (!empty($order_before)) {
        $result = _trade_order_delete($order_before);
      }
      if ($result) {
        $invoke_op = 'delete';
        $order_obj = $order_before;
        $message = t('Order #@num deleted.', array('@num' => $order['order_id']));
        $form_state['redirect'] = 'trade/cart';
      }
    }

    if (!empty($order_obj)) {
      trade_module_invoke($invoke_op, $order_obj);
      if (module_exists('rules')) {
        rules_invoke_event('trade_order_' . $invoke_op, $order_obj);
      }
    }

    if ($invoke_op == 'update' || $invoke_op == 'create') {
      drupal_write_record('trade_order', $order, 'order_id');

      // send mail notify
      $send_result = _trade_order_mail_notify('trade_order_create', $order);
      if (empty($send_result)) {
        $message .= (' ' . t('E-mail notify not sent.'));
      }
      else {
        $message .= (' ' . t('Notify sent on e-mail.'));
      }
    }
    if (!empty($message)) {
      drupal_set_message($message);
    }
  }

}

function trade_order_cart() {
  $build = array();
  if ($order_id = _trade_get_current_order_id()) {
    if ($order = trade_order_load($order_id)) {
      $build = array(
        '#theme' => 'trade_order_edit',
        '#order' => $order
      );
    }
  }
  if (!empty($build)) {
    return $build;
  }
  else {
    drupal_access_denied();
    // drupal_exit();
  }
}

function template_preprocess_trade_option_view(&$vars) {
  $option = $vars['option'];

  if (!empty($option)) {
    $field_instance = field_info_instance('node', $option['option_field'], $vars['entity_bundle']);
    $vars['build'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('trade-option-view', 'trade-option-field-' . $option['option_field'])),
    );
    $vars['build']['content'] = array(
      'label' => array(
        '#theme' => 'html_tag',
        '#tag' => 'label',
        '#value' => $field_instance['label'],
      ),
      'value' => array(
        '#theme' => 'html_tag',
        '#tag' => 'div',
        '#value' => $option['option_title'],
      ),
    );
  }
}

function theme_trade_option_view($vars) {
  return drupal_render($vars['build']);
}

function trade_order_page_view($order) {
  return array('#theme' => 'trade_order_view', '#order' => $order);
}


function template_preprocess_trade_order_view(&$vars) {
  global $language;

  $m_path = drupal_get_path('module', 'trade');
  drupal_add_css($m_path . '/css/trade_order.css');

  $order = $vars['order'];
  if (!empty($vars['options']['view_mode'])) {
    $view_mode = $vars['options']['view_mode'];
  }
  else {
    $view_mode = 'full';
  }

  $title = t('Order #@id', array('@id' => $order->order_id));
  drupal_set_title($title);

  $build['wr'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-order-page', 'trade-order-view'),
    )
  );

  if ($view_mode == 'mail') {
    $build['wr']['#attributes']['style'] = 'width:768px;';
  }

  $build['wr']['date_created'] = array(
    '#type' => 'item',
    '#title' => t('Date created') . ':',
    'field_item' => array(
      '#theme' => 'html_tag',
      '#tag' => 'span',
      '#value' => format_date(strtotime($order->created), 'short'),
    )
  );

  $build['wr']['products'] = trade_order_build_products($order, 'view', $view_mode);

  // build services
  $build['wr']['services'] = trade_order_build_services($order, $build['wr']['products'], 'view', $view_mode);

  $build['wr']['line'] = array(
    '#theme' => 'html_tag',
    '#tag' => 'hr',
  );

  if (!empty($order->amount)) {
    $currency = _trade_get_currency($order->currency);
    $build['wr']['amount_item'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('trade-order-total-all')
      ),
      'amount' => array(
        '#type' => 'item',
        '#title' => t('Total amount') . ':',
        'field_item' => array(
          '#theme' => 'html_tag',
          '#tag' => 'span',
          '#value' => _trade_format_number($order->amount) . ' ' . $currency[1],
        ),
      ),
    );
    if ($view_mode == 'mail') {
      $build['wr']['amount_item']['#attributes']['style'] = 'text-align:left; font-size:1.2rem;';
    }
  }


  $rows = array();
  $cells = array();
  $cell = array();

  $fields = array(
    'name' => t('Full name'),
    'phone' => t('Phone'),
    'mail' => t('E-mail'),
  );

  foreach ($fields as $name => $title) {
    if (!empty($order->{$name})) {
      $element = array(
        '#type' => 'item',
        '#title' => $title . ':',
        'field_item' => array(
          '#theme' => 'html_tag',
          '#tag' => 'span',
          '#value' => $order->{$name},
        )
      );
      $cell[] = drupal_render($element);
    }
  }
  $cells[] = array(
    'data' => implode('', $cell),
    'style' => 'width:20%;vertical-align: top;text-align: left;'
  );

  if (!empty($order->address)) {
    $element = array(
      '#type' => 'item',
      '#title' => t('Address of house') . ':',
      'field_item' => array(
        '#theme' => 'html_tag',
        '#tag' => 'div',
        '#value' => $order->address,
      )
    );
    $cells[] = array('data' => drupal_render($element), 'style' => 'width:40%;vertical-align: top;text-align: left;');
  }

  if (!empty($order->comment)) {
    $element = array(
      '#type' => 'item',
      '#title' => t('Address details') . ':',
      'field_item' => array(
        '#theme' => 'html_tag',
        '#tag' => 'div',
        '#value' => $order->comment,
      ),
    );
    $cells[] = array('data' => drupal_render($element), 'style' => 'width:40%;vertical-align: top;text-align: left;');
  }

  $rows[] = $cells;
  $caption = t('Contact info');
  $build['wr']['order_info'] = array(
    '#prefix' => '<h3>' . $caption . '</h3>',
    '#theme' => 'table',
    '#rows' => $rows,
    '#attributes' => array(
      'class' => array('trade-order-info'),
      'style' => 'vertical-align: top;text-align: left;',
    ),
  );

  if ($view_mode == 'mail') {
    $build['wr']['order_info']['#attributes']['border'] = '1';
    $build['wr']['order_info']['#attributes']['cellpadding'] = '5';
    $build['wr']['order_info']['#attributes']['width'] = '100%';
  }

  $vars['build'] = $build;
}

function theme_trade_order_view($vars) {
  return drupal_render($vars['build']);
}

function trade_orders_page() {
  return array('#theme' => 'trade_orders_page');
}

function template_preprocess_trade_orders_page(&$vars) {
  global $user, $language, $trade_uid;
  $m_path = drupal_get_path('module', 'trade');
  drupal_add_css($m_path . '/css/trade_orders.css');

  $header = array(
    array('data' => t('#'), 'field' => 'tor.order_id'),
    array('data' => t('Date'), 'field' => 'tor.created', 'sort' => 'desc'),
    array('data' => t('Quantity'), 'class' => array('trade-format-number'), 'field' => 'tor.product_count'),
    array('data' => t('Amount'), 'class' => array('trade-format-number'), 'field' => 'tor.amount'),
    array('data' => t('Currency')),
    array('data' => t('Name'), 'field' => 'tor.name'),
    array('data' => t('E-mail'), 'field' => 'tor.mail'),
    array('data' => t('Phone'), 'field' => 'tor.phone'),
    array('data' => t('Operations'))
  );
  if ($user->uid == 1 || user_access('edit own orders') || user_access('edit all orders')) {
    $edit_access = TRUE;
  }
  if (is_null($trade_uid)) {
    $trade_uid = _trade_get_trade_uid();
  }
  $query = db_select('trade_order', 'tor')->fields('tor')
    ->condition('tor.status', 1);


  if (!(arg(1) == 'orders-all' && (user_access('view all orders') || $user->uid == 1))) {
    $query->condition('tor.uid', $user->uid);
    $query->condition('tor.trade_uid', $trade_uid);
  }

  $orders = $query->extend('TableSort')->extend('PagerDefault')->limit(50)->orderByHeader($header)->execute()
    ->fetchAll();

  if (!empty($orders)) {
    $rows = array();

    foreach ($orders as $order) {
      $currency = _trade_get_currency($order->currency);
      $l_opts = array('query' => array('destination' => request_path()));
      $operations = array(
        l(t('View'), "trade/order/{$order->order_id}"),
      );
      if (!empty($edit_access)) {
        $operations[] = l(t('Edit'), "trade/order/{$order->order_id}/edit", $l_opts);
      }
      $rows[] = array(
        $order->order_id,
        format_date(strtotime($order->created), 'short'),
        array('data' => _trade_format_number($order->product_count), 'class' => array('trade-format-number')),
        array('data' => _trade_format_number($order->amount), 'class' => array('trade-format-number')),
        $currency[1],
        $order->name,
        $order->mail,
        $order->phone,
        theme('item_list', array(
          'items' => $operations,
          'attributes' => array('class' => array('trade-order-operations'))
        )),
      );
      // theme_item_list()
    }

    $build['wr'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('trade-orders-page'),
      )
    );
    $build['wr']['content'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => t('Orders not found.')
    );

    $build['wr']['pager'] = array(
      '#theme' => 'pager',
      '#weight' => 5,
    );
    $vars['build'] = $build;
  }
  else {
    drupal_not_found();
    // drupal_exit();
  }
}

function theme_trade_orders_page($vars) {
  return drupal_render($vars['build']);
}

function _trade_order_get_default_values() {
  global $user, $trade_uid;
  if (is_null($trade_uid)) {
    $trade_uid = _trade_get_trade_uid();
  }
  $last_order_complete = db_select('trade_order', 'tor')->fields('tor')
    ->condition('tor.status', 1)
    ->condition('tor.uid', $user->uid)
    ->condition('tor.trade_uid', $trade_uid)
    ->orderBy('tor.created', 'DESC')
    ->execute()->fetchObject();

  return !empty($last_order_complete) ? $last_order_complete : FALSE;
}

function template_preprocess_trade_cart_block(&$vars) {
  global $language;
  $m_path = drupal_get_path('module', 'trade');

  $order_id = $vars['options']['order_id'];
  $build['wr'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-cart-block-inner')
    ),
  );

  $order = trade_order_load($order_id);

  $l_opts = array(
    'html' => TRUE,
  );

  if (empty($order->product_count) && empty($order->amount)) {
    $link_title = '<span class="trade-cart-item-title">' . t('Cart empty') . '</span>';
    $l_opts['attributes'] = array(
      'class' => array('trade-cart-empty')
    );
  }
  else {
    $link_title = '';
    if (!empty($order->product_count)) {
      $quantity_title = t('Quantity');
      $link_title .= '<span class="trade-cart-item-title">' . $quantity_title . ':</span><span class="trade-cart-item-value">' . _trade_format_number($order->product_count) . '</span>';
    }

    if (!empty($order->amount)) {
      $amount_title = t('Amount');
      $link_title .= '<span class="trade-cart-item-title">' . $amount_title . ':</span><span class="trade-cart-item-value">' . _trade_format_number($order->amount) . '</span>';
    }
  }

  $build['wr']['content'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-cart-block-content')
    ),
  );

  $build['wr']['content'] = array(
    '#markup' => l($link_title, 'trade/cart', $l_opts),
  );

  $vars['build'] = $build;
}

function theme_trade_cart_block($vars) {
  return drupal_render($vars['build']);
}


function trade_order_action_edit($order = NULL, $status = FALSE, $order_out = '') {

  // edit order line quantity
  if (!empty($_POST['trade_field_name'])
    && !empty($_POST['trade_order_line_id'])
    && !empty($_POST['trade_order_line_type'])
    && !empty($_POST['trade_order_line_operation'])
  ) {

    if (!empty($_POST['trade_value']) && $_POST['trade_field_name'] == 'quantity' && $_POST['trade_order_line_operation'] == 'edit' && $_POST['trade_order_line_type'] == 'product') {

      // get order line
      if (!empty($order->lines[$_POST['trade_order_line_id']])) {
        $price = $order->lines[$_POST['trade_order_line_id']]->price;
        $quantity = trim($_POST['trade_value']);
        $amount = $price * $quantity;

        $line = array(
          'order_line_id' => $_POST['trade_order_line_id'],
          'quantity' => $quantity,
          'amount' => $amount
        );

        //update order line
        if (drupal_write_record('trade_order_line', $line, 'order_line_id')) {

          // rewrite service
          $result = _trade_order_rewrite_services($order);

          $re_calc_order_amount = TRUE;
        }
      }
    }
    elseif ($_POST['trade_field_name'] == 'order_line_delete' && $_POST['trade_order_line_operation'] == 'delete' && $_POST['trade_order_line_type'] == 'product') {
      // order_line_delete
      if (!empty($order->lines[$_POST['trade_order_line_id']])) {

        //delete
        db_delete('trade_order_line')->condition('order_line_id', $_POST['trade_order_line_id'])->execute();

        // rewrite service
        $result = _trade_order_rewrite_services($order);

        $re_calc_order_amount = TRUE;

      }
    }
    elseif (!empty($_POST['trade_value']) && $_POST['trade_order_line_operation'] == 'edit' && $_POST['trade_order_line_type'] == 'service') {
      // service
      // service shipping
      if ($_POST['trade_order_line_id'] == 'shipping' || $_POST['trade_order_line_id'] == 'lifting' || $_POST['trade_order_line_id'] == 'assembly') {

        $product_services = _trade_get_products_services($order);

        if (!empty($order->service)) {
          $order_service = unserialize($order->service);
        }
        else {
          $order_service = array();
        }

        $service_update_values = array();
        $service_update_values[$_POST['trade_order_line_id']][$_POST['trade_field_name']] = $_POST['trade_value'];

        $order_service = _trade_order_service_update_values($product_services, $order_service, $service_update_values);

        $fields = array(
          'order_id' => $order->order_id,
          'service' => $order_service,
          'changed' => format_date(REQUEST_TIME, 'custom', 'c'),
        );

        // SAVED_UPDATED
        $result = drupal_write_record('trade_order', $fields, 'order_id');
        if ($result) {
          $re_calc_order_amount = TRUE;
        }
      }
    }

    //re calc order amount
    if (!empty($re_calc_order_amount)) {
      if (_trade_order_rewrite_quantity_and_amount($order->order_id)) {
        $update_cart_block = TRUE;
        $order = trade_order_load($order->order_id);
        $status = TRUE;
        $order_out = theme('trade_order_edit', array('order' => $order, 'options' => array('hidden' => TRUE)));
      }
    }
  }

  $output = array(
    'status' => $status,
    'order' => $order_out
  );
  //cart block
  if (!empty($update_cart_block)) {
    $render_var = trade_block_view('cart');
    $output['cart'] = drupal_render($render_var);
  }

  drupal_json_output($output);
}
