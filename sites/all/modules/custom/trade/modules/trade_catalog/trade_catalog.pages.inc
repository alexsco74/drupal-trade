<?php

/**
 * @file
 * Page callbacks for the trade catalog module.
 */

function trade_catalog_pages_catalog_page() {
  return theme('trade_catalog_pages_catalog_page');
}

function theme_trade_catalog_pages_catalog_page($vars) {
  return drupal_render($vars['build']);
}

function template_preprocess_trade_catalog_pages_catalog_page(&$vars) {

  //include css
  $m_path = drupal_get_path('module', 'trade_catalog');
  drupal_add_css($m_path . '/css/trade_catalog.css');
  if (!module_exists('trade')) {
    drupal_add_js($m_path . '/js/trade.js');
    drupal_add_css($m_path . '/css/trade_ajax_load.css');
  }

  // include masonry
  if (variable_get('trade_catalog_enable_masonry', TRUE)) {
    $masonry_path = libraries_get_path('masonry');
    if ($masonry_path) {
      drupal_add_js($masonry_path . '/masonry.pkgd.min.js');
      drupal_add_js($masonry_path . '/imagesloaded.pkgd.min.js');
      drupal_add_js($masonry_path . '/jquery.easing.1.3.js');
    }
    drupal_add_js($m_path . '/js/trade_catalog_page.js');
    $masonry_exists = TRUE;
  }
  else {
    $masonry_exists = FALSE;
  }

  $current_path = current_path();
  $term = _trade_catalog_term_load($current_path);

  if (empty($term)) {
    $catalog_voc_mn = _trade_catalog_get_voc_mn();
    $catalog_voc = taxonomy_vocabulary_machine_name_load($catalog_voc_mn);

    if (empty($catalog_voc)) {
      drupal_not_found();
      // drupal_exit();
    }
  }

  if (!empty($term)) {
    if (module_exists('i18n_taxonomy')) {
      $term->name = i18n_taxonomy_term_name($term);
    }

    $title = $term->name;
    $catalog_voc = taxonomy_vocabulary_load($term->vid);
  }
  else {
    if (!empty($catalog_voc->name)) {
      $title = $catalog_voc->name;
    }
    else {
      drupal_set_message(t('Catalog vocabulary not found. Please check you config on page - admin/config/trade/catalog'));
    }
  }


  drupal_set_title($title);


  $id = drupal_html_id('trade-catalog-page-wr');
  $build = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-catalog-page-wr'),
      'id' => $id,
    ),
  );


  //add children terms block
  if (empty($_GET['search'])) {

    if (variable_get('trade_product_catalog_page_terms_block_en', FALSE)) {
      $build['#prefix'] = theme('trade_catalog_terms_block', array(
          'options' => array(
            'term' => $term,
            'voc' => $catalog_voc,
          )
        )
      );
    }

    // term descriptions

    $build_element = array();
    $element = array();
    $l_query = array(
      'query' => array('destination' => current_path()),
    );
    // $field_description_items = field_get_items('taxonomy_term', $term, 'field_description_footer');
    if (!empty($term) && module_exists('i18n_taxonomy')) {
      $term_description = i18n_taxonomy_term_description($term);
    }
    else {
      $term_description = !empty($term->description) ? $term->description : '';
    }
    if (!empty($term->description)) {
      $build_element['inner'] = array(
        '#markup' => $term_description,
        '#weight' => 1,
      );

    }

    if (!empty($term) && taxonomy_term_edit_access($term) && isset($term->description)) {
      $build_element['edit_link'] = array(
        '#prefix' => '<div class="trade-edit-link-wr">',
        '#suffix' => '</div>',
        '#markup' => l(t('Edit'), "taxonomy/term/{$term->tid}/edit", $l_query + array('attributes' => array('class' => array('trade-edit-link')))),
        '#weight' => -1,
      );
    }
    if (!empty($build_element)) {
      $trade_catalog_term_description_exists = TRUE;
      $element['description'] = array(
        '#prefix' => '<div class="trade-catalog-term-description">',
        '#suffix' => '</div>',
      );
      $element['description']['element'] = $build_element;
      $build['#suffix'] = render($element);
    }

  }
  else {
    $build['#prefix'] = theme('trade_catalog_search_block');
  }

// term page
  if (!empty($term)) {

    // Build breadcrumb based on the hierarchy of the term.
    $current = (object) array(
      'tid' => $term->tid,
    );
    $breadcrumb = array();
    while ($parents = taxonomy_get_parents($current->tid)) {
      $current = array_shift($parents);
      $breadcrumb[] = l($current->name, 'taxonomy/term/' . $current->tid);
    }
    $breadcrumb[] = l($catalog_voc->name, TRADE_CATALOG_CATALOG_PAGE);
    $breadcrumb[] = l(t('Home'), NULL);
    $breadcrumb = array_reverse($breadcrumb);
    drupal_set_breadcrumb($breadcrumb);

    $l_query = array(
      'query' => array('destination' => current_path()),
    );
    // description header
    $build_element = array();
    $field_description_items = field_get_items('taxonomy_term', $term, 'field_description_header');
    if (!empty($field_description_items[0]['value'])) {
      $build_element['inner'] = array(
        '#markup' => $field_description_items[0]['value'],
        '#weight' => 1,
      );

    }
    if (!empty($term) && taxonomy_term_edit_access($term) && isset($term->field_description_header)) {

      $build_element['edit_link'] = array(
        '#prefix' => '<div class="trade-edit-link-wr">',
        '#suffix' => '</div>',
        '#markup' => l(t('Edit'), "taxonomy/term/{$term->tid}/edit", $l_query + array('attributes' => array('class' => array('trade-edit-link')))),
        '#weight' => -1,
      );
    }
    if (!empty($build_element)) {
      $build['description_header'] = array(
        '#prefix' => '<div class="trade-catalog-term-description-header">',
        '#suffix' => '</div>',
        '#weight' => -1,
      );
      $build['description_header'] += $build_element;
    }

    // description footer
    $build_element = array();
    $field_description_items = field_get_items('taxonomy_term', $term, 'field_description_footer');
    if (!empty($field_description_items[0]['value'])) {
      $build_element['inner'] = array(
        '#markup' => $field_description_items[0]['value'],
        '#weight' => 1,
      );

    }
    if (empty($trade_catalog_term_description_exists)) {
      if (!empty($term) && taxonomy_term_edit_access($term) && isset($term->field_description_footer)) {
        $build_element['edit_link'] = array(
          '#prefix' => '<div class="trade-edit-link-wr">',
          '#suffix' => '</div>',
          '#markup' => l(t('Edit'), "taxonomy/term/{$term->tid}/edit", $l_query + array('attributes' => array('class' => array('trade-edit-link')))),
          '#weight' => -1,
        );
      }
      if (!empty($build_element)) {
        $build['description_footer'] = array(
          '#prefix' => '<div class="trade-catalog-term-description-footer">',
          '#suffix' => '</div>',
          '#weight' => 10,
        );
        $build['description_footer'] += $build_element;
      }
    }
  }
  else {
    //catalog
    $breadcrumb = array(
      l($catalog_voc->name, TRADE_CATALOG_CATALOG_PAGE),
      l(t('Home'), NULL)
    );
  }

  $build['wr'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-catalog-page')
    ),
    'content' => array(
      '#type' => 'container',
      '#weight' => 2,
      '#attributes' => array(
        'class' => array('trade-catalog-page-content')
      ),
    )
  );

  $filters_fields = _trade_catalog_get_filters();
  $types = array_filter(variable_get('trade_product_node_types', array('product')));
  $catalog_field = variable_get('trade_product_catalog_field', 'field_product_catalog');
  $catalog_terms = array();
  $filters_values = array();

  if (!empty($filters_fields) && !empty($types) && !empty($catalog_field)) {
    unset($filters_fields[$catalog_field]);
  }
  else {
    drupal_not_found();
    // drupal_exit();
  }

// term & children on term page
  if (!empty($term->tid)) {
    $catalog_terms = array($term->tid => $term);

    if (!variable_get('trade_product_catalog_select_product_current_term', FALSE)) {
      $tree = taxonomy_get_tree($term->vid, $term->tid);

      if (!empty($tree)) {
        foreach ($tree as $key => $tree_term) {
          $catalog_terms[$tree_term->tid] = $tree_term;
        }
      }
    }
  }

//get sort
  $sort = array();
  $sort_fields = _trade_catalog_sort_fields_elements(1);

  if (!empty($_GET['sort']) && !empty($sort_fields)) {
    foreach ($sort_fields as $sort_fields_data) {
      if ($sort_fields_data['name'] == trim($_GET['sort'])) {
        $sort['field'] = $sort_fields_data['name'];
        if (!empty($_GET['direction']) && in_array($_GET['direction'], array(
            'asc',
            'desc'
          ), TRUE)
        ) {
          $sort['direction'] = $_GET['direction'];
        }
        else {
          $sort['direction'] = $sort_fields_data['direction'];
        }
      }
    }
  }

  //set default sort
  if (empty($sort['field'])) {
    foreach ($sort_fields as $sort_fields_data) {
      if ($sort_fields_data['default']) {
        $sort['field'] = $sort_fields_data['name'];
        $sort['direction'] = $sort_fields_data['direction'];
        break;
      }
    }
  }

// get search
  $search = _trade_catalog_get_search();

// get filters values
  $filters_values = _trade_catalog_get_filters_values();

  $query_content = _trade_catalog_get_nodes_query($types, $filters_fields, $catalog_field, $catalog_terms, $filters_values, TRUE, $sort, $search);
  $count_on_page = variable_get('trade_product_catalog_page_count_nodes', 0);

  //commerce price update
  if (module_exists('commerce') && module_exists('custom_ccp') && (user_access('administer site configuration') || user_access('product price update'))) {
    $query_content_all = clone $query_content;
    $nids_all = $query_content_all->execute()->fetchCol();
    if (!empty($nids_all)) {
      $nids_all = array_unique($nids_all);
      $build['custom_ccp'] = array(
        '#type' => 'container',
        '#weight' => -11,
        '#attributes' => array(
          'class' => array('trade-catalog-pages-custom-cpp-wr')
        ),
        'title' => array(
          '#type' => 'html_tag',
          '#tag' => 'label',
          '#value' => t('Change product price')
        ),
        'count' => array(
          '#type' => 'item',
          '#title' => t('Selected products count') . ' = ',
          'num' => array('#markup' => count($nids_all)),
        ),
        'inner' => array(drupal_get_form('trade_catalog_custom_cpp_form', $nids_all))
      );
    }
  }


  if (!empty($count_on_page)) {
    $nids = $query_content->extend('PagerDefault')
      ->limit($count_on_page)
      ->execute()
      ->fetchCol();
  }
  elseif ($current_path == TRADE_CATALOG_CATALOG_PAGE) {
    $nids = $query_content->extend('PagerDefault')
      ->limit(100)
      ->execute()
      ->fetchCol();
  }
  else {
    $nids = $query_content->execute()->fetchCol();
  }
  if (!empty($nids)) {
    $nids = array_unique($nids);
    $nodes = node_load_multiple($nids);

    // build sort links
    $current_path = current_path();
    $query = $_GET;
    unset($query['q'], $query['sort'], $query['direction']);

    $sort_items = array();
    $sort_options = array();

    // build sort items
    foreach ($sort_fields as $sort_fields_data) {
      $sort_field = $sort_fields_data['name'];
      $sort_title = $sort_fields_data['title'];
      if (!empty($sort['field']) && $sort['field'] == $sort_field) {
        if ($sort['direction'] == 'asc') {
          $sort_options[$sort_field]['attributes'] = array(
            'class' => array('trade-sort-active', 'trade-sort-asc'),
          );
          $sort_options[$sort_field]['query'] = $query + array(
              'sort' => $sort_field,
              'direction' => 'desc',
            );
        }
        elseif ($sort['direction'] == 'desc') {
          $sort_options[$sort_field]['attributes'] = array(
            'class' => array('trade-sort-active', 'trade-sort-desc'),
          );
          $sort_options[$sort_field]['query'] = $query + array(
              'sort' => $sort_field,
              'direction' => 'asc',
            );
        }
      }
      else {
        $sort_options[$sort_field]['attributes'] = array();
        $sort_options[$sort_field]['query'] = $query + array(
            'sort' => $sort_field,
            'direction' => $sort_fields_data['direction'],
          );
      }
      $data_ajax_select = variable_get('trade_product_data_ajax_wrapper_selector', '');
      if (empty($data_aja_select)) {
        $data_ajax_select = '#' . $id;
      }
      if (!module_exists('dc_ajax_add_cart')) {
        $sort_options[$sort_field]['attributes']['data-ajax-sel'] = $data_ajax_select;
      }
      $sort_items[] = l('<span class="trade-inner">' . $sort_title . '</span>', $current_path, $sort_options[$sort_field] + array('html' => TRUE));
    }

    $build['sort'] = array(
      '#type' => 'container',
      '#weight' => -10,
      '#attributes' => array('class' => array('trade-catalog-page-sort')),
      'inner' => array(
        '#type' => 'container',
        '#attributes' => array('class' => array('trade-catalog-page-sort-inner')),
        'list' => array(
          '#title' => t('Sort unde') . ':',
          '#theme' => 'item_list',
          '#items' => $sort_items,
        )
      )
    );

    if (module_exists('trade')) {

      //with product cache
      $build['wr']['content'] += _trade_node_view_multiple($nodes);
    }
    else {
      $build['wr']['content'] += node_view_multiple($nodes);
    }
    if ($masonry_exists) {
      $build['wr']['content']['#attributes']['class'][] = 'trade-opacity';
    }
    if (!empty($count_on_page) || $current_path == TRADE_CATALOG_CATALOG_PAGE) {
      $build['wr']['pager'] = array(
        '#theme' => 'pager',
        '#weight' => 3,
      );
    }
  }
  else {
    if (isset($build['#suffix'])) {
      $suffix = $build['#suffix'];
    }
    else {
      $suffix = '';
    }
    $build = array(
      '#prefix' => '<div class="trade-catalog-page-only-terms-wr">',
      '#markup' => $build['#prefix'],
      '#suffix' => '</div>' . $suffix,
    );
    // unset($build['description_header'],$build['description_footer'], $build['wr']);
//    $build['wr']['content']['empty'] = array(
//      '#prefix' => '<p>',
//      '#markup' => t('Product not found. Please, change or clear filers.'),
//      '#suffix' => '</p>',
//    );
  }

  $vars['build'] = $build;
}

function _trade_catalog_get_catalog_uri_from_filters($filters = array()) {
  $uri = '';
  $term = _trade_catalog_term_load(current_path());

  if (!empty($term)) {
    $term_alias = drupal_get_path_alias('taxonomy/term/' . $term->tid);
    $uri = TRADE_CATALOG_CATALOG_PAGE . "/{$term_alias}" . (!empty($filters) ? ('/filters/' . implode('/', $filters)) : '');
  }
  return $uri;
}

function trade_catalog_search_form($form, &$form_state) {
  $text = _trade_catalog_get_search();
  $form['text'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 32,
    '#default_value' => !empty($text) ? $text : '',
    '#attributes' => array(
      'placeholder' => t('Input search string'),
      'required' => 'required',
    )
  );
  $form['submit'] = array(
    '#prefix' => '<div class="trade-actions-wr">',
    '#suffix' => '</div>',
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  return $form;
}

function trade_catalog_search_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['text'])) {
    $text = trim(filter_xss($form_state['values']['text']));
    if (!empty($text)) {
      $query = $_GET;
      unset($query['q']);
      $query['search'] = $text;
      $form_state['redirect'] = array(
        TRADE_CATALOG_CATALOG_PAGE,
        array('query' => $query)
      );
    }
  }
}


function _trade_catalog_get_search() {
  $search = '';

  if (!empty($_GET['search'])) {
    $search = trim(filter_xss($_GET['search'], array()));
  }

  return $search;
}

/**
 * Processes a checkboxes form element.
 */
function trade_catalog_process_checkboxes($element) {
  global $language;
  // get filters values
  $filters_values = _trade_catalog_get_filters_values();
  $new_filter_values = array();

  $value = !empty($element['#value']) && is_array($element['#value']) ? $element['#value'] : array();
  $element['#tree'] = TRUE;
  if (!empty($element['#options']) && count($element['#options']) > 0) {
    if (!isset($element['#default_value']) || $element['#default_value'] == 0) {
      $element['#default_value'] = array();
    }
    $weight = 0;
    foreach ($element['#options'] as $key => $choice) {
      if ($key === 0) {
        $key = '0';
      }
      $weight += 0.001;

      $element += array($key => array());
      $element[$key] += array(
        '#type' => 'checkbox',
        '#title' => $choice,
        '#return_value' => $key,
        '#default_value' => isset($value[$key]) ? $key : NULL,
        '#attributes' => $element['#attributes'],
        '#ajax' => isset($element['#ajax']) ? $element['#ajax'] : NULL,
        '#weight' => $weight,
      );

      $element[$key]['#attributes']['title'] = $choice;

      if (!empty($element['#custom_bfe']['#options'][$key]['#attributes'])) {
        $element[$key]['#attributes'] = array_merge($element[$key]['#attributes'], $element['#custom_bfe']['#options'][$key]['#attributes']);
      }

      //set count with page link catalog with filter page
      if (!empty($element['#custom_bfe']['#options'][$key]['#count'])) {
        $show_link_use = FALSE;
        $remove_link_user = FALSE;
        $link_href = '';

        // empty filter
        if (empty($filters_values[$element['#name']])) {
          $new_filter_values = $filters_values;
          $new_filter_values[$element['#name']] = array($key);
          $show_link_use = TRUE;
        }
        else {
          // add value to filter
          $values_keys = array_combine($filters_values[$element['#name']], $filters_values[$element['#name']]);
          if (!isset($values_keys[$key])) {
            $new_filter_values = $filters_values;
            $new_filter_values[$element['#name']][] = $key;
            $show_link_use = TRUE;
          }
          else {
            // remove value from filter
            $filter_value_key = array_search($key, $filters_values[$element['#name']]);

            if ($filter_value_key !== FALSE) {
              $new_filter_values = $filters_values;
              unset($new_filter_values[$element['#name']]);
              $remove_link_user = TRUE;
            }
          }
        }

        if ($show_link_use || $remove_link_user) {
          $filters = _trade_catalog_get_filters_plain_from_filters_values($new_filter_values, 0);
          $link_href = _trade_catalog_get_catalog_uri_from_filters($filters);
        }

        // if (!empty($link_href) && !(count($element['#options']) == 1 && $element['#custom_bfe']['#options'][$key]['#count'] == 1)) {
        if (!empty($link_href)) {

          $element[$key]['#attributes']['data-ajax-href'] = url($link_href);
          $element[$key]['#attributes']['data-ajax-sel'] = '.trade-catalog-filters';
          if (!empty($show_link_use)) {
            $link_options = array(
              'html' => TRUE,
              'attributes' => array(
                'class' => array(
                  'trade-catalog-link',
                  'trade-catalog-show-link'
                )
              ),
            );
            $element[$key]['#description'] = '<span class="trade-catalog-gradient"></span>' . l(t('Show: @count', array('@count' => $element['#custom_bfe']['#options'][$key]['#count'])), $link_href, $link_options);
          }
          else {
            $link_options = array(
              'attributes' => array(
                'class' => array(
                  'trade-catalog-link',
                  'trade-catalog-remove-link'
                )
              ),
            );
            $element[$key]['#description'] = '<span class="trade-catalog-gradient"></span>' . l(t('Remove: @count', array('@count' => $element['#custom_bfe']['#options'][$key]['#count'])), $link_href, $link_options);
          }

        }
        else {
          $element[$key]['#description'] = "({$element['#custom_bfe']['#options'][$key]['#count']})";
        }
      }
    }
  }

  return $element;
}

/*
 *  rewrite trade catalog page
 */
function _trade_catalog_pages_taxonomy_term($term) {
  if (_trade_catalog_is_catalog_term_page()) {
    $uri = 'catalog/' . current_path();
    menu_set_active_item($uri);
    return menu_execute_active_handler(NULL, FALSE);
  }
  else {
    module_load_include('inc', 'taxonomy', 'taxonomy.pages');
    return taxonomy_term_page($term);
  }
}

function _trade_catalog_get_nodes_query($types = array(), $fields = array(), $catalog_field = '', $catalog_terms = array(), $filters_values = array(), $dist = FALSE, $sort = array(), $search = '') {
  global $language;

  //product fields
  $catalog_product_fields = _trade_catalog_product_get_fields(TRUE);

  $trade_catalog_filters_get_dimensions_fields = _trade_catalog_filters_get_dimensions_fields();
  $field_info_fields = field_info_fields();
  $price_join_exists = FALSE;
  $field_table_numeric_slider = array();
  $field_value_price = '';

  $field_price_from_name = '';
  $field_price_to_name = '';
  $field_price_type = '';

  if (module_exists('trade')) {
    $field_price_from_name = 'field_price__from';
    $field_price_to_name = 'field_price__to';
    $field_price_type = 'trade';
  }
  elseif (module_exists('commerce')) {
    $commerce_product_field = variable_get('trade_catalog_commerce_product_field', '');
    $commerce_price_field = variable_get('trade_catalog_commerce_product_price_field', '');
    if (!empty($commerce_product_field) && !empty($commerce_price_field)) {
      $field_price_from_name = $commerce_price_field . '_amount' . '__from';
      $field_price_to_name = $commerce_price_field . '_amount' . '__to';
      $field_price_type = 'commerce';
    }
  }

  $query = db_select('node', 'n')
    ->condition('n.status', 1)
    ->condition('n.type', $types, 'IN')
    ->condition(db_or()
      ->condition('n.language', $language->language)
      ->condition('n.language', 'NONE')
    )
    ->fields('n');

  if (!empty($search)) {

    // inner body
    $body_field = 'body';
    $body_table = 'field_data_' . $body_field;
    $body_field_value = $body_field . '_value';
    $query->leftJoin($body_table, $body_field, "{$body_field}.entity_id = n.nid AND {$body_field}.entity_type = :{$body_field}_et", array(":{$body_field}_et" => 'node'));

    // commerce sku search
    if ($field_price_type == 'commerce') {
      $field_table_sku = 'commerce_product';
      $field_value_sku = 'sku';
      $field_table_product_search = 'field_data_' . $commerce_product_field;
      $field_value_product_search = $commerce_product_field . '_product_id';

      $query->innerJoin($field_table_product_search, $field_table_product_search, "{$field_table_product_search}.entity_id = n.nid");
      $query->leftJoin($field_table_sku, $field_table_sku, "{$field_table_sku}.product_id = {$field_table_product_search}.{$field_value_product_search}");

      $query->condition(db_or()
        ->condition("{$body_field}.{$body_field_value}", '%' . $search . '%', 'LIKE')
        ->condition('n.title', '%' . $search . '%', 'LIKE')
        ->condition("{$field_table_sku}.{$field_value_sku}", '%' . $search . '%', 'LIKE')
      );
    }
    else {
      $query->condition(db_or()
        ->condition("{$body_field}.{$body_field_value}", '%' . $search . '%', 'LIKE')
        ->condition('n.title', '%' . $search . '%', 'LIKE')
      );
    }
  }


  if ($dist) {
    $query->groupBy('n.nid');
  }

  // catalog field
  if (!empty($catalog_field) && !empty($catalog_terms)) {
    $field = $catalog_field;
    $field_table = 'field_data_' . $field;
    $field_value = $field . '_tid';
    $query->innerJoin($field_table, $field, "{$field}.entity_id = n.nid AND {$field}.entity_type = :{$field}_et", array(":{$field}_et" => 'node'));
    $query->condition("{$field}.{$field_value}", array_keys($catalog_terms), 'IN');
  }


  // if ($commerce_product_field && $commerce_price_field) {
  foreach ($fields as $field) {
    $field_explode = explode('__', $field);
    if ($field == $field_price_from_name) {
      switch ($field_price_type) {
        case 'trade':
          $field_table_price = 'trade_price';
          $field_value_price = 'price';
          if (!empty($filters_values[$field])) {
            $filters_values_field = current($filters_values[$field]);
            $query->innerJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
            $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '>=');
          }
          else {
            $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
          }
          $query->addField($field_table_price, $field_value_price);
          $price_join_exists = TRUE;
          break;
        case 'commerce':
          $field_table_price = 'field_data_' . $commerce_price_field;
          $field_value_price = $commerce_price_field . '_amount';
          $field_table_product = 'field_data_' . $commerce_product_field;
          $field_value_product = $commerce_product_field . '_product_id';
          if (!empty($filters_values[$field])) {
            $filters_values_field = current($filters_values[$field]) * 100;
            $query->innerJoin($field_table_product, $field_table_product, "{$field_table_product}.entity_id = n.nid");
            $query->innerJoin($field_table_price, $field_table_price, "{$field_table_price}.entity_id = {$field_table_product}.{$field_value_product}");
            $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '>=');
          }
          else {
            $query->innerJoin($field_table_product, $field_table_product, "{$field_table_product}.entity_id = n.nid");
            $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.entity_id = {$field_table_product}.{$field_value_product}");
          }
          $query->addField($field_table_price, $field_value_price);
          $price_join_exists = TRUE;
          break;
      }
    }
    elseif ($field == $field_price_to_name) {
      switch ($field_price_type) {
        case 'trade':
          if (empty($field_table_price)) {
            $field_table_price = 'trade_price';
            $field_value_price = 'price';
            if (!empty($filters_values[$field])) {
              $filters_values_field = current($filters_values[$field]);
              $query->innerJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
              $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '<=');
            }
            else {
              $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
            }
            $query->addField($field_table_price, $field_value_price);
            $price_join_exists = TRUE;

          }
          else {
            // add only condition __to
            if (!empty($filters_values[$field])) {
              $filters_values_field = current($filters_values[$field]);
              $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '<=');
            }
          }
          break;
        case 'commerce':
          if (empty($field_table_price)) {
            $field_table_price = 'field_data_' . $commerce_price_field;
            $field_value_price = $commerce_price_field . '_amount';
            $field_table_product = 'field_data_' . $commerce_product_field;
            $field_value_product = $commerce_product_field . '_product_id';

            if (!empty($filters_values[$field])) {
              $filters_values_field = current($filters_values[$field]) * 100;
              $query->innerJoin($field_table_product, $field_table_product, "{$field_table_product}.entity_id = n.nid");
              $query->innerJoin($field_table_price, $field_table_price, "{$field_table_price}.entity_id = {$field_table_product}.{$field_value_product}");
              $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '<=');
            }
            else {
              $query->innerJoin($field_table_product, $field_table_product, "{$field_table_product}.entity_id = n.nid");
              $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.entity_id = {$field_table_product}.{$field_value_product}");
            }
            $query->addField($field_table_price, $field_value_price);
            $price_join_exists = TRUE;

          }
          else {
            // add only condition __to
            if (!empty($filters_values[$field])) {
              $filters_values_field = current($filters_values[$field]) * 100;
              $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '<=');
            }
          }
          break;
      }
    }
    elseif (!empty($field_explode) && end($field_explode) == 'from') {
      // __from
      $field_table_numeric_slider_name = 'field_data_' . str_replace('__from', '', $field);
      $field_table_numeric_slider_value = str_replace('__from', '', $field) . '_value';

      $field_table_numeric_slider[$field_table_numeric_slider_name] = $field_table_numeric_slider_value;

      if (!empty($filters_values[$field])) {
        $filters_values_field = current($filters_values[$field]);
        $query->innerJoin($field_table_numeric_slider_name, $field_table_numeric_slider_name, "{$field_table_numeric_slider_name}.entity_type = 'node' AND {$field_table_numeric_slider_name}.entity_id = n.nid");
        $query->condition("{$field_table_numeric_slider_name}.{$field_table_numeric_slider_value}", $filters_values_field, '>=');
      }
      else {
        $query->leftJoin($field_table_numeric_slider_name, $field_table_numeric_slider_name, "{$field_table_numeric_slider_name}.entity_type = 'node' AND {$field_table_numeric_slider_name}.entity_id = n.nid");
      }
      $query->addField($field_table_numeric_slider_name, $field_table_numeric_slider_value);
      // $price_join_exists = TRUE;
    }
    elseif (!empty($field_explode) && end($field_explode) == 'to') {
      // __to
      $field_table_numeric_slider_name = 'field_data_' . str_replace('__to', '', $field);
      $field_table_numeric_slider_value = str_replace('__to', '', $field) . '_value';
      if (empty($field_table_numeric_slider[$field_table_numeric_slider_name])) {
        $field_table_numeric_slider[$field_table_numeric_slider_name] = $field_table_numeric_slider_value;
        if (!empty($filters_values[$field])) {
          $filters_values_field = current($filters_values[$field]);
          $query->innerJoin($field_table_numeric_slider_name, $field_table_numeric_slider_name, "{$field_table_numeric_slider_name}.entity_type = 'node' AND {$field_table_numeric_slider_name}.entity_id = n.nid");
          $query->condition("{$field_table_numeric_slider_name}.{$field_table_numeric_slider_value}", $filters_values_field, '<=');

          // $query->innerJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
          // $query->condition("{$field_table_price}.{$field_value_price}", $filters_values_field, '<=');
        }
        else {
          // $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
          $query->leftJoin($field_table_numeric_slider_name, $field_table_numeric_slider_name, "{$field_table_numeric_slider_name}.entity_type = 'node' AND {$field_table_numeric_slider_name}.entity_id = n.nid");
        }
        $query->addField($field_table_numeric_slider_name, $field_table_numeric_slider_value);
      }
      else {
        // add only condition __to
        if (!empty($filters_values[$field])) {
          $filters_values_field = current($filters_values[$field]);
          $query->condition("{$field_table_numeric_slider_name}.{$field_table_numeric_slider_value}", $filters_values_field, '<=');
        }
      }
    }
    elseif (strpos($field, 'field_') === 0) {

      if (isset($field_info_fields[$field])) {
        switch ($field_info_fields[$field]['type']) {
          case 'taxonomy_term_reference':
            // term_reference field
            $field_table = 'field_data_' . $field;
            if (db_table_exists($field_table)) {
              $field_value = $field . '_tid';
              if (!empty($filters_values[$field])) {
                $query->innerJoin($field_table, $field, " {$field}.entity_id = n.nid AND {$field}.entity_type = :{$field}_et", array(":{$field}_et" => 'node'));
                $query->condition("{$field}.{$field_value}", $filters_values[$field], 'IN');
              }
              else {
                $query->leftJoin($field_table, $field, "{$field}.entity_id = n.nid AND {$field}.entity_type = :{$field}_et", array(":{$field}_et" => 'node'));
              }
              $query->addField($field, $field_value);
            }
            break;
          case 'text':
            //text
            // term_reference field
            $field_table = 'field_data_' . $field;
            if (db_table_exists($field_table)) {
              $field_value = $field . '_value';
              if (!empty($filters_values[$field])) {
                $query->innerJoin($field_table, $field, " {$field}.entity_id = n.nid AND {$field}.entity_type = :{$field}_et", array(":{$field}_et" => 'node'));
                $query->condition("{$field}.{$field_value}", $filters_values[$field], 'IN');
              }
              else {
                $query->leftJoin($field_table, $field, "{$field}.entity_id = n.nid AND {$field}.entity_type = :{$field}_et", array(":{$field}_et" => 'node'));
              }
              $query->addField($field, $field_value);
            }
            break;
        }
      }


    }
  }

  if (!$price_join_exists) {
    switch ($field_price_type) {
      case 'trade':
        $field_table_price = 'trade_price';
        $field_value_price = 'price';
        $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.nid = n.nid");
        $query->addField($field_table_price, $field_value_price);
        break;
      case 'commerce':

        $field_table_price = 'field_data_' . $commerce_price_field;
        $field_value_price = $commerce_price_field . '_amount';
        $field_table_product = 'field_data_' . $commerce_product_field;
        $field_value_product = $commerce_product_field . '_product_id';

        $query->innerJoin($field_table_product, $field_table_product, "{$field_table_product}.entity_id = n.nid");
        $query->leftJoin($field_table_price, $field_table_price, "{$field_table_price}.entity_id = {$field_table_product}.{$field_value_product}");
        $query->addField($field_table_price, $field_value_price);
        break;
    }
  }

  // dimensions table wo filters add need
  foreach ($trade_catalog_filters_get_dimensions_fields as $dim_field_key => $dim_field_value) {
    if (strpos($dim_field_key, '__from')) {
      $dim_table_field = str_replace('__from', '', $dim_field_key);
      $dim_table_field_name = 'field_data_' . $dim_table_field;
      if (empty($field_table_numeric_slider[$dim_table_field_name])) {
        $dim_table_field_value = $dim_table_field . '_value';
        $query->leftJoin($dim_table_field_name, $dim_table_field_name, "{$dim_table_field_name}.entity_type = 'node' AND {$dim_table_field_name}.entity_id = n.nid");
        $query->addField($dim_table_field_name, $dim_table_field_value);
      }
    }
  }

  // sort

  if (!empty($sort['field'])) {

    // && (in_array($sort['field'], array('created', 'title'), TRUE) || !empty($catalog_product_fields[$sort['field']]))
    if ($sort['field'] == 'price') {

      // sort on price
      $sort_direction = !empty($sort['direction']) && in_array($sort['direction'], array(
        'asc',
        'desc'
      ), TRUE) ? drupal_strtoupper($sort['direction']) : 'ASC';
      $query->orderBy("{$field_table_price}.{$field_value_price}", $sort_direction);
    }
    elseif (in_array($sort['field'], array('created', 'title'), TRUE)) {

      //sort node table fields
      $sort_direction = !empty($sort['direction']) && in_array($sort['direction'], array(
        'asc',
        'desc'
      ), TRUE) ? drupal_strtoupper($sort['direction']) : 'ASC';
      $query->orderBy("n.{$sort['field']}", $sort_direction);
    }
    elseif (!empty($catalog_product_fields[$sort['field']])) {

      //node fields
      $catalog_product_fields_info = field_info_field($sort['field']);

      // sort field numeric
      if (!empty($catalog_product_fields_info['type']) && in_array($catalog_product_fields_info['type'], array(
          'number_integer',
          'number_decimal',
          'text'
        ), TRUE)
      ) {

        $sort_field = $sort['field'];
        $sort_table = 'field_data_' . $sort_field;
        $sort_table_alias = $sort_table . '_sort';
        $sort_table_field_value = $sort_field . '_value';

        $sort_direction = !empty($sort['direction']) && in_array($sort['direction'], array(
          'asc',
          'desc'
        ), TRUE) ? drupal_strtoupper($sort['direction']) : 'ASC';

        if (db_table_exists($sort_table)) {
          $query->leftJoin($sort_table, $sort_table_alias, "{$sort_table_alias}.entity_id = n.nid AND {$sort_table_alias}.entity_type = :{$sort_table_alias}_et", array(":{$sort_table_alias}_et" => 'node'));
          $query->addField($sort_table_alias, $sort_table_field_value);
          $query->orderBy("{$sort_table_alias}.{$sort_table_field_value}", $sort_direction);
        }
      }
      elseif (!empty($catalog_product_fields_info['type']) && $catalog_product_fields_info['type'] == 'taxonomy_term_reference') {

        // sort field term
        $sort_field = $sort['field'];
        $sort_table = 'field_data_' . $sort_field;
        $sort_table_alias = $sort_table . '_sort';
        $sort_table_field_value = $sort_field . '_tid';
        $sort_term_data_table_alias = $sort_table_alias . '_ttd';

        $sort_direction = !empty($sort['direction']) && in_array($sort['direction'], array(
          'asc',
          'desc'
        ), TRUE) ? drupal_strtoupper($sort['direction']) : 'ASC';

        if (db_table_exists($sort_table)) {
          $query->leftJoin($sort_table, $sort_table_alias, "{$sort_table_alias}.entity_id = n.nid AND {$sort_table_alias}.entity_type = :{$sort_table_alias}_et", array(":{$sort_table_alias}_et" => 'node'));
          $query->leftJoin('taxonomy_term_data', $sort_term_data_table_alias, "{$sort_term_data_table_alias}.tid = {$sort_table_alias}.{$sort_table_field_value}");
          $query->addField($sort_term_data_table_alias, 'weight');
          $query->addField($sort_term_data_table_alias, 'name');
          $query->orderBy("{$sort_term_data_table_alias}.weight", $sort_direction);
          $query->orderBy("{$sort_term_data_table_alias}.name", $sort_direction);
        }
      }
    }
  }

  return $query;
}

function _trade_catalog_get_filters_plain_from_filters_values($values, $mode = 1) {
  $filters = array();
  $filters_field = _trade_catalog_get_filters();

  $values = array_intersect_key($values, $filters_field);

  $field_price = '';
  if (module_exists('commerce')) {
    $commerce_product_field = variable_get('trade_catalog_commerce_product_field', '');
    $commerce_price_field = variable_get('trade_catalog_commerce_product_price_field', '');
    if (!empty($commerce_product_field) && !empty($commerce_price_field)) {
      $field_price = $commerce_price_field;
    }
  }

  foreach ($values as $filter_name => $filter_values) {
    if (strpos($filter_name, 'field_') === 0 ||
      (!empty($field_price) && strpos($filter_name, $field_price) === 0)
    ) {

      if (!is_array($filter_values)) {
        $filter_arr_values = array($filter_values);
      }
      else {
        $filter_arr_values = $filter_values;
      }

      // clearing values
      foreach ($filter_arr_values as &$filter_value) {
        $filter_value = _trade_catalog_filter_value2get_value(trim(filter_xss($filter_value, array())));
      }
      $filter_arr_values = array_filter($filter_arr_values);
      if (!empty($filter_arr_values)) {
        $filters[] = "{$filter_name}-" . implode('-', $filter_arr_values);
      }
    }
  }

  return $filters;
}

function _trade_catalog_get_filters() {
  $filters_fields = &drupal_static(__FUNCTION__);
  $field_info_fields = field_info_fields();

  if (!isset($filters_fields)) {
    if (module_exists('trade')) {
      $filters_fields = array(
        'field_price__from' => 'field_price__from',
        'field_price__to' => 'field_price__to',
      );
    }
    elseif (module_exists('commerce')) {
      $commerce_product_field = variable_get('trade_catalog_commerce_product_field', '');
      $commerce_price_field = variable_get('trade_catalog_commerce_product_price_field', '');
      if ($commerce_product_field && $commerce_price_field) {
        $filters_fields = array(
          $commerce_price_field . '_amount' . '__from' => $commerce_price_field . '_amount' . '__from',
          $commerce_price_field . '_amount' . '__to' => $commerce_price_field . '_amount' . '__to',
        );
      }
    }

    //dimensions fields
    $catalog_dimensions_filter_slider_fields = _trade_catalog_filters_get_dimensions_fields();
    $filters_fields += $catalog_dimensions_filter_slider_fields;
    $filters_fields += array_filter(variable_get('trade_product_catalog_filter_fields', array()));
    $catalog_field = variable_get('trade_product_catalog_field', 'field_product_catalog');
    unset($filters_fields[$catalog_field]);

    //number_integer type change on __from __to
    $field_info_fields_construct = array();
    foreach ($filters_fields as $field_key => $field_value) {
      if (isset($field_info_fields[$field_key]['type'])
        && ($field_info_fields[$field_key]['type'] == 'number_integer' || $field_info_fields[$field_key]['type'] == 'number_decimal')
      ) {
        $field_info_fields_construct[$field_key . '__from'] = $field_value . '__from';
        $field_info_fields_construct[$field_key . '__to'] = $field_value . '__to';
      }
      else {
        $field_info_fields_construct[$field_key] = $field_value;
      }
    }
    $filters_fields = $field_info_fields_construct;
  }

  return $filters_fields;
}

function trade_catalog_filter_form($form, &$form_state, $fields_values = array(), $filters_values = array(), $fields_enable_values = array()) {
  $m_path = drupal_get_path('module', 'trade_catalog');
  $trade_catalog_filters_get_dimensions_fields = _trade_catalog_filters_get_dimensions_fields('label');
  $field_info_fields = field_info_fields();
  $filter_fields = _trade_catalog_get_filters();

  $field_info_field_map = field_info_field_map();

  $field_price_key = '';
  if (module_exists('trade')) {
    $field_price_key = 'price';
    $data_field = 'field-price';
  }
  elseif (module_exists('commerce')) {
    $commerce_product_field = variable_get('trade_catalog_commerce_product_field', '');
    $commerce_price_field = variable_get('trade_catalog_commerce_product_price_field', '');
    if (!empty($commerce_product_field) && $commerce_price_field) {
      $field_price_key = $commerce_price_field . '_amount';
      $data_field = str_replace('_', '-', $field_price_key);
    }
  }

  foreach ($fields_values as $key => $values) {
    if (strpos($key, '_tid')) {
      // term
      $field_name = str_replace('_tid', '', $key);
      $field_info = field_info_field($field_name);

      $options = array();
      if (!empty($field_info['settings']['allowed_values'][0]['vocabulary'])) {
        $voc = taxonomy_vocabulary_machine_name_load($field_info['settings']['allowed_values'][0]['vocabulary']);
        $tree = taxonomy_get_tree($voc->vid);
        foreach ($tree as $term) {
          if (module_exists('i18n_taxonomy')) {
            $term_name = i18n_taxonomy_term_name($term);
          }
          else {
            $term_name = $term->name;
          }
          $options[$term->tid] = $term_name; // $term->name;
        }
      }

      if (!empty($options)) {
        if (module_exists('i18n_taxonomy')) {
          $voc_name = i18n_taxonomy_vocabulary_name($voc);
        }
        else {
          $voc_name = $voc->name;
        }

        $options = array_intersect_key($options, $values);
        $field_instance = array();
        $field_label = '';
        if (!empty($field_info_field_map[$field_name]['bundles']['node'][0])) {
          $field_instance = field_info_instance('node', $field_name, $field_info_field_map[$field_name]['bundles']['node'][0]);
          $field_label = $field_instance['label'];
          if (function_exists('i18n_string_object_translate')) {
            $field_instance_translate = i18n_string_object_translate('field_instance', $field_instance);
            if (!empty($field_instance_translate['label'])) {
              $field_label = $field_instance_translate['label'];
            }
          }
        }

        $form[$field_name] = array(
          '#type' => 'checkboxes',
          '#title' => !empty($field_label) ? $field_label : $voc_name,
          '#options' => $options,
          '#process' => array(
            'form_process_checkboxes',
            'trade_catalog_process_checkboxes'
          ),
        );
        if (!empty($filters_values[$field_name])) {
          $form[$field_name]['#default_value'] = $filters_values[$field_name];
        }
        if (!empty($options)) {
          $element_children_attributes = array();
          foreach ($options as $opt_key => $opt_title) {
            if (empty($fields_enable_values[$field_name . '_tid'][$opt_key])) {
              $element_children_attributes[$opt_key]['#attributes']['disabled'] = 'disabled';
            }
            else {
              $element_children_attributes[$opt_key]['#count'] = count($fields_enable_values[$field_name . '_tid'][$opt_key]);
              // $element_children_attributes[$opt_key]['#nodes'] = $fields_enable_values[$field_name . '_tid'][$opt_key];
            }
          }

          if (!empty($element_children_attributes)) {
            $form[$field_name]['#custom_bfe']['#options'] = $element_children_attributes;
          }
        }
      }

    }
    elseif ($key == $field_price_key) {
      // price

      if (!empty($values)) {

        asort($values);
        $data = array();
        $data['min'] = current($values);
        $data['max'] = end($values);
        if (module_exists('commerce')) {
          $data['min'] = $data['min'] / 100;
          $data['max'] = $data['max'] / 100;
        }
        if (intVal($data['min']) != $data['min']) {
          $data['min'] = intVal($data['min']);
        }
        if (intVal($data['max']) != $data['max']) {
          $data['max'] = intVal($data['max']) + 1;
        }
        if (!empty($fields_enable_values[$key])) {
          ksort($fields_enable_values[$key]);
          $enable_prices = array_keys($fields_enable_values[$key]);
          $data['enable-min'] = current($enable_prices);
          $data['enable-max'] = end($enable_prices);
          if (module_exists('commerce')) {
            $data['enable-min'] = $data['enable-min'] / 100;
            $data['enable-max'] = $data['enable-max'] / 100;
          }
          if (intVal($data['enable-min']) != $data['enable-min']) {
            $data['enable-min'] = intVal($data['enable-min']);
          }
          if (intVal($data['enable-max']) != $data['enable-max']) {
            $data['enable-max'] = intVal($data['enable-max']) + 1;
          }
        }

        if (isset($data['enable-min']) && isset($data['enable-max'])) {
          $form['price'] = array(
            '#type' => 'container',
            'field_title' => array(
              '#theme' => 'html_tag',
              '#tag' => 'label',
              '#value' => t('Price'),
              '#attributes' => array('class' => array('trade-field-title')),
            ),
            '#attributes' => array(
              'class' => array(
                'trade-' . $data_field . '-form-item',
                'trade-numeric-slider-form-item'
              ),
              'data-field' => $data_field,
            ),
            '#attached' => array(
              // Add Farbtastic color picker.
              'library' => array(
                array('system', 'ui.slider'),
              ),
              // Add custom CSS.
              'css' => array(
                $m_path . '/css/trade_slider.css',
              ),
              // Add custom JavaScript.
              'js' => array(
                $m_path . '/js/trade_slider.js',
              ),
            ),
          );

          foreach ($data as $data_key => $data_val) {
            $form['price']['#attributes']['data-' . $data_key] = $data_val;
          }


          if ($field_price_key == 'price') {
            $price_fields = array(
              'field_price__from' => t('From'),
              'field_price__to' => t('Unto')
            );
          }
          else {
            $price_fields = array(
              $field_price_key . '__from' => t('From'),
              $field_price_key . '__to' => t('Unto')
            );
          }

          foreach ($price_fields as $price_key => $price_title) {

            $placeholder = '';

            if (!empty($filters_values[$price_key])) {
              $default_price_value = current($filters_values[$price_key]);
            }
            else {
              unset($default_price_value);
            }

            if (empty($default_price_value)) {
              if (strpos($price_key, '__from')) {
                if (!empty($data['enable-min'])) {
                  $default_price_value = '';
                  $placeholder = $data['enable-min'];
                }
              }
              else {
                // max
                if (!empty($data['enable-max'])) {
                  $default_price_value = '';
                  $placeholder = $data['enable-max'];
                }
              }
            }
            else {
              //check less min & check more max
              if (!empty($data['enable-min']) && $default_price_value < $data['enable-min']) {
                $default_price_value = $data['enable-min'];
              }
              if (!empty($data['enable-max']) && $default_price_value > $data['enable-max']) {
                $default_price_value = $data['enable-max'];
              }
            }


            $form['price'][$price_key] = array(
              '#type' => 'textfield',
              '#title' => $price_title,
              '#default_value' => !empty($default_price_value) ? $default_price_value : '',
              '#size' => 14,
              '#maxlength' => 16,
            );


            if (!empty($placeholder)) {
              $form['price'][$price_key]['#attributes']['placeholder'] = $placeholder;
            }
          }

          $currency_abbr = variable_get('trade_currency', 'RUB');

          if (module_exists('trade')) {
            $currency = _trade_get_currency($currency_abbr);
          }
          elseif (module_exists('commerce')) {
            $commerce_currency = commerce_currency_load(commerce_default_currency());
            $currency = array('1' => $commerce_currency['symbol']);
          }

          $form['price']['currency'] = array(
            '#theme' => 'html_tag',
            '#tag' => 'label',
            '#value' => $currency[1],
            '#attributes' => array('class' => array('trade-field-suffix')),
          );

          // slider
          $form['price']['slider'] = array(
            '#type' => 'container',
            '#attributes' => array(
              'class' => array(
                'trade-' . $data_field . '-slider-widget',
                'trade-numeric-slider-widget'
              )
            ),
          );
        }
      }
    }
    elseif (strpos($key, '_value')) {
      $field_name_single = str_replace('_value', '', $key);

      //field number_integer & number decimal
      if ((isset($filter_fields[$field_name_single . '__from']) && isset($filter_fields[$field_name_single . '__to']))) {

        if (isset($field_info_fields[$field_name_single]['bundles']['node'][0])) {
          $node_bundle = $field_info_fields[$field_name_single]['bundles']['node'][0];
          $field_info_instance = field_info_instance('node', $field_name_single, $node_bundle);
          if (!empty($field_info_instance['label'])) {
            if (module_exists('i18n_field')) {
              $field_title_single = i18n_field_translate_property($field_info_instance, 'label');
            }
            else {
              $field_title_single = $field_info_instance['label'];
            }
          }
          else {
            $field_title_single = $field_name;
          }
          // $field_title_single = !empty($field_info_instance['label']) ? $field_info_instance['label'] : $field_name_single;
        }

        $field_name_single_uri = str_replace('_', '-', $field_name_single);

        if (!empty($values)) {
          asort($values);
          $data = array();
          $data['min'] = current($values);
          $data['max'] = end($values);

          if (intVal($data['min']) !== $data['min']) {
            $data['min'] = intVal($data['min']);
          }
          if (intVal($data['max']) !== $data['max']) {
            $data['max'] = intVal($data['max']) + 1;
          }
          if (!empty($fields_enable_values[$key])) {
            ksort($fields_enable_values[$key]);
            $enable_dim_values = array_keys($fields_enable_values[$key]);
            $data['enable-min'] = current($enable_dim_values);
            $data['enable-max'] = end($enable_dim_values);
            if (intVal($data['enable-min']) !== $data['enable-min']) {
              $data['enable-min'] = intVal($data['enable-min']);
            }
            if (intVal($data['enable-max']) !== $data['enable-max']) {
              $data['enable-max'] = intVal($data['enable-max']) + 1;
            }
          }

          if (isset($data['enable-min']) && isset($data['enable-max'])) {
            $form[$field_name_single] = array(
              '#type' => 'container',
              'field_title' => array(
                '#theme' => 'html_tag',
                '#tag' => 'label',
                '#value' => $field_title_single,
                '#attributes' => array('class' => array('trade-field-title')),
              ),
              '#attributes' => array(
                'class' => array(
                  'trade-' . $field_name_single_uri . '-form-item',
                  'trade-numeric-slider-form-item'
                ),
                'data-field' => $field_name_single_uri,
              ),
              '#attached' => array(
                // Add Farbtastic color picker.
                'library' => array(
                  array('system', 'ui.slider'),
                ),
                // Add custom CSS.
                'css' => array(
                  $m_path . '/css/trade_slider.css',
                ),
                // Add custom JavaScript.
                'js' => array(
                  $m_path . '/js/trade_slider.js',
                ),
              ),
            );

            foreach ($data as $data_key => $data_val) {
              $form[$field_name_single]['#attributes']['data-' . $data_key] = $data_val;
            }

            $price_fields = array(
              $field_name_single . '__from' => t('From'),
              $field_name_single . '__to' => t('Unto')
            );

            $value_exists = FALSE;

            foreach ($price_fields as $price_key => $price_title) {

              $placeholder = '';

              if (isset($filters_values[$price_key])) {
                $default_price_value = current($filters_values[$price_key]);
              }
              else {
                unset($default_price_value);
              }


              if (empty($default_price_value)) {
                if ($price_key == $field_name_single . '__from') {
                  if (isset($data['enable-min'])) {
                    $default_price_value = '';
                    $placeholder = $data['enable-min'];
                  }
                }
                else {
                  // max
                  if (isset($data['enable-max'])) {
                    $default_price_value = '';
                    $placeholder = $data['enable-max'];
                  }
                }
              }
              else {
                //check less min & check more max
                if (isset($data['enable-min']) && $default_price_value < $data['enable-min']) {
                  $default_price_value = $data['enable-min'];
                }
                if (isset($data['enable-max']) && $default_price_value > $data['enable-max']) {
                  $default_price_value = $data['enable-max'];
                }
              }
              if (!empty($default_price_value)) {
                $value_exists = TRUE;
              }

              $form[$field_name_single][$price_key] = array(
                '#type' => 'textfield',
                '#title' => $price_title,
                '#default_value' => isset($default_price_value) ? $default_price_value : '',
                '#size' => 14,
                '#maxlength' => 16,
                '#attributes' => array('class' => array('trade-numeric-slider-value-field'))
              );

              if (isset($placeholder)) {
                $form[$field_name_single][$price_key]['#attributes']['placeholder'] = $placeholder;
              }
            }

            if (!empty($field_info_instance['settings']['suffix'])) {
              if (module_exists('i18n_field')) {
                $suffix_item_value = i18n_field_translate_property($field_info_instance, 'suffix');
              }
              else {
                $suffix_item_value = $field_info_instance['settings']['suffix'];
              }
            }
            elseif (!empty($field_info_instance['description'])) {
              if (module_exists('i18n_field')) {
                $suffix_item_value = i18n_field_translate_property($field_info_instance, 'description');
              }
              else {
                $suffix_item_value = $field_info_instance['description'];
              }
            }
            elseif (strpos($field_name_single, '_guarantee')) {
              $suffix_item_value = t('year');
            }
            else {
              $suffix_item_value = '';
            }

            $form[$field_name_single]['currency'] = array(
              '#theme' => 'html_tag',
              '#tag' => 'label',
              '#value' => $suffix_item_value,
              '#attributes' => array('class' => array('trade-field-suffix')),
            );

            // slider
            $form[$field_name_single]['slider'] = array(
              '#type' => 'container',
              '#attributes' => array(
                'class' => array(
                  'trade-' . $field_name_single_uri . '-slider-widget',
                  'trade-numeric-slider-widget'
                )
              ),
            );
            if ($value_exists) {
              $form[$field_name_single]['#attributes']['class'][] = 'trade-container-is-open';
            }
            else {
              $form[$field_name_single]['#attributes']['class'][] = 'trade-container-is-close';
            }
          }
        }
      }
      elseif ($field_info_fields[$field_name_single]['type'] == 'text') {

        $field_name = $field_name_single;

        if (!empty($field_info_fields[$field_name]['bundles']['node'][0])) {
          $node_bundle = $field_info_fields[$field_name]['bundles']['node'][0];
          $field_ins = field_info_instance('node', $field_name, $node_bundle);
        }
        if (!empty($values)) {
          $options = $values;

          if (!empty($field_ins['label'])) {
            if (module_exists('i18n_field')) {
              $element_title = i18n_field_translate_property($field_ins, 'label');
            }
            else {
              $element_title = $field_ins['label'];
            }
          }
          else {
            $element_title = $field_name;
          }

          $form[$field_name] = array(
            '#type' => 'checkboxes',
            '#title' => $element_title,
            '#options' => $options,
            '#process' => array(
              'form_process_checkboxes',
              'trade_catalog_process_checkboxes'
            ),
          );
          if (!empty($filters_values[$field_name])) {
            $form[$field_name]['#default_value'] = $filters_values[$field_name];
          }
          if (!empty($options)) {
            $element_children_attributes = array();
            foreach ($options as $opt_key => $opt_title) {
              if (empty($fields_enable_values[$field_name . '_value'][$opt_key])) {
                $element_children_attributes[$opt_key]['#attributes']['disabled'] = 'disabled';
              }
              else {
                $element_children_attributes[$opt_key]['#count'] = count($fields_enable_values[$field_name . '_value'][$opt_key]);
                // $element_children_attributes[$opt_key]['#nodes'] = $fields_enable_values[$field_name . '_tid'][$opt_key];
              }
            }

            if (!empty($element_children_attributes)) {
              $form[$field_name]['#custom_bfe']['#options'] = $element_children_attributes;
            }
          }
        }
        //todo end
      }
    }
  }
  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-actions'),
    ),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
  );
  $form['actions']['clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear'),
  );
  return $form;

}

function trade_catalog_filter_form_submit($form, &$form_state) {

  $values = array_filter($form_state['values']);
  if ($values['op'] == t('Clear')) {
    $filters = array();
  }
  else {
    $filters = _trade_catalog_get_filters_plain_from_filters_values($values);
  }

  $redirect_uri = _trade_catalog_get_catalog_uri_from_filters($filters);

  if (!empty($redirect_uri)) {
    $form_state['redirect'] = $redirect_uri;
  }
}

function template_preprocess_trade_catalog_search_block(&$vars) {
  $vars['build'] = array(
    'wr' => array(
      '#type' => 'container',
      '#attributes' => array('class' => array('trade-catalog-search')),
      'form' => drupal_get_form('trade_catalog_search_form'),
    ),
  );
}

function theme_trade_catalog_search_block($vars) {
  return drupal_render($vars['build']);
}

function template_preprocess_trade_catalog_terms_block(&$vars) {
  $vars['build'] = array();
  $voc = $vars['options']['voc'];

  $parent = $vars['options']['term'];

  if (!empty($vars['options']['term']->tid)) {
    $parent = $vars['options']['term']->tid;
    $max_depth = 1;
  }
  else {
    $parent = 0;
    $max_depth = 2;
  }

  $tree = taxonomy_get_tree($voc->vid, $parent, $max_depth);
  $items = array();
  $tids = array();
  $term_render = array();
  foreach ($tree as $tree_key => $term) {
    if (module_exists('i18n')) {
      $term_name = i18n_taxonomy_term_name($term);
    }
    else {
      $term_name = $term->name;
    }
    if ($term->parents[0] && isset($items[$term->parents[0]])) {
      if (!empty($items[$term->parents[0]]['children'])) {
        if (!empty($items[$term->parents[0]]['children']['more_link'])) {
          continue;
        }
        else {
          $count = count($items[$term->parents[0]]['children']);
        }
      }
      if (!empty($count) && $count == 5) {
        $items[$term->parents[0]]['children']['more_link'] = array(
          'data' => l(t('All list'), "taxonomy/term/{$term->parents[0]}"),
          'class' => array('trade-catalog-term-all-list-link'),
        );
      }
      else {
        $items[$term->parents[0]]['children'][$term->tid]['data'] = l($term_name, "taxonomy/term/{$term->tid}");
      }
    }
    else {

      if (empty($items[$term->tid]['data'])) {
        //variable_get('trade_product_catalog_tree_term_is', 'thumbnail')

        $tids[] = $term->tid;
        $items[$term->tid]['data'] = $term->tid; //l($term_name, "taxonomy/term/{$term->tid}");
      }
    }
  }


  if ($items && $tids) {
    $view_mode = variable_get('trade_product_catalog_tree_view_mode', 'default');
    if (empty($view_mode)) {
      $view_mode = 'default';
    }
    $terms = taxonomy_term_load_multiple($tids);
    $terms_view = taxonomy_term_view_multiple($terms, $view_mode);

    foreach ($items as $tid => &$data) {
      if (!empty($term_render[$tid])) {
        $data['data'] = $term_render[$tid];
      }
      elseif (!empty($terms_view['taxonomy_terms'][$tid])) {
        $term_render[$tid] = drupal_render($terms_view['taxonomy_terms'][$tid]);
        $data['data'] = $term_render[$tid];
      }
    }

    $vars['build'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('trade-catalog-terms'),
      ),
      'content' => array(
        '#theme' => 'trade_catalog_item_list',
        '#items' => $items,
        '#attributes' => array('class' => array('trade-catalog-terms-list'))
      ),
    );
  }
}

/**
 * see theme_item_list
 */
function theme_trade_catalog_item_list($variables) {
  $m_path = drupal_get_path('module', 'trade_catalog');
  drupal_add_css($m_path . '/css/trade_catalog.css');
  $items = $variables['items'];
  $title = $variables['title'];
  $type = $variables['type'];
  $attributes = $variables['attributes'];

  $output = '<div class="trade-catalog-item-list">';
  $attributes['class'][] = 'trade-catalog-item-list-inner';
  if (isset($title) && $title !== '') {
    $output .= '<h3>' . $title . '</h3>';
  }

  if (!empty($items)) {
    $output .= "<$type" . drupal_attributes($attributes) . '>';
    $num_items = count($items);
    $i = 0;
    foreach ($items as $item) {
      $attributes = array();
      $children = array();
      $data = '';
      $i++;
      if (is_array($item)) {
        foreach ($item as $key => $value) {
          if ($key == 'data') {
            $data = $value;
          }
          elseif ($key == 'children') {
            $children = $value;
          }
          else {
            $attributes[$key] = $value;
          }
        }
      }
      else {
        $data = $item;
      }
      if (count($children) > 0) {
        // Render nested list.
        $data .= theme_trade_catalog_item_list(array(
          'items' => $children,
          'title' => NULL,
          'type' => $type,
          'attributes' => $attributes
        ));
      }
      if ($i == 1) {
        $attributes['class'][] = 'first';
      }
      if ($i == $num_items) {
        $attributes['class'][] = 'last';
      }
      $output .= '<div' . drupal_attributes($attributes) . '>' . $data . "</div>\n";
    }
    $output .= "</$type>";
  }
  $output .= '</div>';
  return $output;
}

function theme_trade_catalog_terms_block($vars) {
  return drupal_render($vars['build']);
}

function template_preprocess_catalog_filters_block(&$vars) {
  $vars['build'] = array();

  $filters_fields = _trade_catalog_get_filters();

  $types = array_filter(variable_get('trade_product_node_types', array('product')));
  $catalog_field = variable_get('trade_product_catalog_field', 'field_product_catalog');


  if (!empty($filters_fields) && !empty($types) && !empty($vars['options']['catalog_terms']) && !empty($catalog_field)) {

    $query = _trade_catalog_get_nodes_query($types, $filters_fields, $catalog_field, $vars['options']['catalog_terms']);
    $nodes = $query->execute()->fetchAll();


    if (!empty($nodes)) {

      $fields_values = _trade_catalog_get_fields_values_from_nodes($nodes);

      if (!empty($fields_values)) {
        $vars['build'] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('trade-catalog-filters'),
          ),
          '#attached' => array(
            'js' => array(drupal_get_path('module', 'trade_catalog') . '/js/trade_catalog_filter.js')
          ),
        );

        // get filters values
        $filters_values = _trade_catalog_get_filters_values();

        $fields_enable_values_query = _trade_catalog_get_nodes_query($types, $filters_fields, $catalog_field, $vars['options']['catalog_terms'], $filters_values);
        $fields_enable_values_nodes = $fields_enable_values_query->execute()
          ->fetchAll();

        $fields_enable_values = array();
        if (!empty($fields_enable_values_nodes)) {
          $fields_enable_values = _trade_catalog_get_fields_values_from_nodes($fields_enable_values_nodes, TRUE);
        }
        $form_var = drupal_get_form('trade_catalog_filter_form', $fields_values, $filters_values, $fields_enable_values);
        $vars['build']['filter_form'] = array(
          '#markup' => drupal_render($form_var),
        );
      }
    }
  }
}

function theme_catalog_filters_block($vars) {
  return drupal_render($vars['build']);
}

function _trade_catalog_get_filters_values() {
  $filters_values = array();
  $curr_path = current_path();
  $start_filters = strpos($curr_path, '/filters/');
  if (!empty($start_filters)) {
    // $filters_str = drupal_substr($curr_path, ($start_filters + drupal_strlen('/filters/')));
    $filters_str_ex = explode('/filters/', $curr_path);
    if (!empty($filters_str_ex[1])) {
      $filters_str = $filters_str_ex[1];
    }
    if (!empty($filters_str)) {
      $filters_arr = explode('/', $filters_str);
      if (!empty($filters_arr)) {
        $filters_field = _trade_catalog_get_filters();

        foreach ($filters_arr as &$filter_str) {

          $filter_str = trim(filter_xss($filter_str, array()));
          if (!empty($filter_str)) {
            foreach ($filters_field as $filter_name) {
              $filter_values = array();

              if (strpos($filter_str, $filter_name . '-') === 0) {
                $filter_value_str = str_replace($filter_name . '-', '', $filter_str);
                if (!empty($filter_value_str)) {

                  $filter_value_arr = explode('-', $filter_value_str);
                  foreach ($filter_value_arr as &$filter_value) {
                    $filter_value = trim($filter_value);
                    if (!empty($filter_value)) {
                      $filter_values[] = _trade_catalog_get_value2filter_value($filter_value);
                    }
                  }
                }
              }
              if (!empty($filter_values)) {
                $filters_values[$filter_name] = $filter_values;
              }
            }
          }
        }
      }
    }
  }

  return $filters_values;
}


function _trade_catalog_get_fields_values_from_nodes($nodes = array(), $with_count = FALSE) {
  $fields_values = array();

  $field_price_key = '';
  if (module_exists('trade')) {
    $field_price_key = 'price';
  }
  elseif (module_exists('commerce')) {
    $commerce_product_field = variable_get('trade_catalog_commerce_product_field', '');
    $commerce_price_field = variable_get('trade_catalog_commerce_product_price_field', '');
    if (!empty($commerce_product_field) && !empty($commerce_price_field)) {
      $field_price_key = $commerce_price_field . '_amount';
    }
  }

  foreach ($nodes as $node) {
    // fields values
    foreach ($node as $field_key => $field_value) {
      if (strpos($field_key, 'field_') === 0 || $field_key == $field_price_key) {
        if (!empty($field_value)) {
          if ($with_count) {
            $fields_values[$field_key][$field_value][$node->nid] = $node->nid;
          }
          elseif (empty($fields_values[$field_key][$field_value])) {
            $fields_values[$field_key][$field_value] = $field_value;
          }
        }
      }
    }
  }

  return $fields_values;
}

function trade_catalog_custom_cpp_form($form, &$form_state, $nids = array()) {
  $form = array();
  $form['wr'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('container-inline'),
    ),
  );
  $form['nids'] = array(
    '#type' => 'hidden',
    '#value' => serialize($nids),
  );
  $form['wr']['op_type'] = array(
    '#type' => 'select',
    '#title' => t('Operation type'),
    '#default_value' => 'multiple',
    '#options' => array(
      'set' => '=',
      'multiple' => '*',
      'plus' => '+',
      'minus' => '-'
    ),
  );
  $form['wr']['int_part'] = array(
    '#type' => 'textfield',
    '#size' => 8,
    '#maxlength' => 8,
    '#title' => 'Integer part',
    '#title_display' => 'invisible',
    '#default_value' => '1',
    '#element_validate' => array('element_validate_number')
  );

  $form['wr']['sep'] = array(
    '#markup' => '.',
  );

  $form['wr']['dd_part'] = array(
    '#type' => 'textfield',
    '#size' => 2,
    '#maxlength' => 2,
    '#default_value' => '00',
    '#title' => 'Digital part',
    '#title_display' => 'invisible',
    '#element_validate' => array('element_validate_number')
  );

  //$precision
  //The optional number of decimal digits to round to
  $form['wr']['op_precision'] = array(
    '#type' => 'select',
    '#title' => t('Operation precision'),
    '#default_value' => 'multiple',
    '#default_value' => '0',
    '#options' => array('0' => '0', '1' => '1', '2' => '2'),
  );
  $form['wr']['change'] = array(
    '#type' => 'submit',
    '#value' => 'Update',
  );
  return $form;
}

function trade_catalog_custom_cpp_form_validate($form, $form_state) {
  if (empty($form_state['values']['int_part']) && empty($form_state['values']['dd_part'])) {
    form_set_error('int_part', t('Enter integer number'));
    form_set_error('dd_part');
  }
}

function trade_catalog_custom_cpp_form_submit($form, &$form_state) {
  global $language;
  if ($nids = unserialize($form_state['values']['nids'])) {
    $_SESSION['http_request_count'] = 0;

    $cont = array(
      'op_type',
      'int_part',
      'dd_part',
      'op_precision'
    );
    $cont = array_intersect_key($form_state['values'], array_combine($cont, $cont));

    $batch = _trade_catalog_custom_cpp_batch($nids, $cont);
    batch_set($batch);
  }

  else {
    $mes = ('Field for external code not found. Import not start.');
    drupal_set_message($mes);
  }
}