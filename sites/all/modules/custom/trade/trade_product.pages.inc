<?php

/**
 * @file
 * Product page callbacks for the trade order module.
 */


function trade_product_upholstery_json_callback($node = NULL, $status = FALSE, $data = '') {
  $build = array();
  $markup = '';


  if (!empty($node) && !empty($_POST['trade_nid'])) {
    $markup = _trade_get_upholstery_markup($node);

    $status = TRUE;
  }

  if (empty($markup)) {
    $markup = drupal_render($build);
  }

  $output = array(
    'status' => $status,
    'data' => $markup,
  );

  drupal_json_output($output);
}

function trade_product_upholstery_materials_json_callback($node = NULL, $trade_category_collection = NULL, $status = FALSE, $data = '') {
  $markup = '';


  if (!empty($_POST['trade_nid']) && !empty($node) && !empty($trade_category_collection)) {

    $markup_temp = _trade_product_uphosltery_build($node, 'full', $trade_category_collection->category, $trade_category_collection->collection);

    if (!empty($markup_temp['#markup'])) {
      $markup_temp['#markup'] = str_replace('</div></form>', '', $markup_temp['#markup']);
      $start_ul = strpos($markup_temp['#markup'], '<ul');

      if ($start_ul) {
        $ul = drupal_substr($markup_temp['#markup'], $start_ul);
        $end_index = strpos($ul, '</ul>');
        $remove_str = substr($ul, ($end_index + 5));
        $ul = str_replace($remove_str, '', $ul);
      }

    }
    if (!empty($ul)) {
      $markup = $ul;
    }
    $status = TRUE;
  }

  $output = array(
    'status' => $status,
    'data' => $markup,
  );

  drupal_json_output($output);
}