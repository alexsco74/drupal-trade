<?php

/**
 * @file
 * Page callbacks for the trade module.
 */

function template_preprocess_trade_product_image_gallery(&$vars) {
  $m_path = drupal_get_path('module', 'trade');

  $lib_path = libraries_get_path('flexslider');
  if ($lib_path) {
    drupal_add_js($lib_path . '/jquery.flexslider-min.js');
    drupal_add_css($lib_path . '/flexslider.css');
  }

  //inc jquery.easing.js
  $lib_path = libraries_get_path('easing');
  if ($lib_path) {
    drupal_add_js($lib_path . '/jquery.easing.js');
  }

  //include cycle2
  $lib_path = libraries_get_path('cycle2');
  if ($lib_path) {
    drupal_add_js($lib_path . '/build/jquery.cycle2.js');
    drupal_add_js($lib_path . '/src/jquery.cycle2.carousel.js');
  }

// cycle test
  $vars['build'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'trade-product-images-gallery',
      )
    ),
    '#attached' => array(
      'css' => array(
        $m_path . '/css/trade.css',
        $m_path . '/css/trade_gallery.css',
      ),
      'js' => array(
        $m_path . '/js/trade_gallery.js' => array('scope' => 'footer'),
      )
    )
  );
  $vars['build']['images_gallery_table'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'trade-product-images-gallery-table',
        'trade-table'
      )
    ),
  );
  $vars['build']['images_gallery_table']['row'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'trade-product-images-gallery-table-row',
        'trade-table-row'
      )
    ),
  );
  $vars['build']['images_gallery_table']['row']['cell_left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'trade-product-images-gallery-table-row-cell-left',
        'trade-table-cell'
      )
    ),
  );


  // preview gallery
  $item_list = array();
  foreach ($vars['items'] as $item) {
    // theme_image_formatter()
    $item_build = array(
      '#theme' => 'image_formatter',
      '#item' => $item,
      '#image_style' => $vars['sett']['image_preview_is'],
      '#path' => array(
        'path' => file_create_url($item['uri']),
        'options' => array(
          'attributes' => array(
            'class' => array('colorbox'),
            'rel' => 'trade-product-images'
          )
        ),
      )
    );
    $item_list[] = drupal_render($item_build) . '<a href="#" class="trade-product-image-popup-magnifier"></a>';
  }

  $vars['build']['images_gallery_table']['row']['cell_left']['gallery_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => drupal_html_id('trade-product-images-gallery-slider'),
      'class' => array(
        'trade-product-images-gallery-table-row-cell-left-gallery-container',
        'flexslider',
        'trade-product-images-gallery-slider',
      )
    ),
  );
  // theme_item_list()
  $vars['build']['images_gallery_table']['row']['cell_left']['gallery_container']['ul'] = array(
    '#theme' => 'item_list',
    '#items' => $item_list,
    '#attributes' => array(
      'class' => array(
        'slides'
      ),
    )
  );

  $vars['build']['images_gallery_table']['row']['cell_right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'trade-product-images-gallery-table-row-cell-right',
        'trade-table-cell'
      )
    ),
  );
  // thumbnail gallery
  $item_list = array();
  // theme_image_formatter()

  foreach ($vars['items'] as $item) {
    $item_build = array(
      '#theme' => 'image_formatter',
      '#item' => $item,
      '#image_style' => $vars['sett']['image_thumb_is'],
    );
    $item_build['#item']['attributes']['class'][] = 'trade-product-gallery-nav-link';
    $item_list[] = drupal_render($item_build);
  }

  $vars['build']['images_gallery_table']['row']['cell_right']['gallery_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => drupal_html_id('trade-product-images-gallery-carousel'),
      'class' => array(
        'trade-product-images-gallery-table-row-cell-right-gallery-container',
        'trade-product-images-gallery-carousel',
      )
    ),
  );

  // cycle
  $carousel_next = drupal_html_id('trade-carousel-next');
  $carousel_prev = drupal_html_id('trade-carousel-prev');
  $count_cycle_item = count($item_list);
  $image_nav_view_count = $vars['sett']['image_nav_view_count'];
  if ($count_cycle_item > $image_nav_view_count) {
    $cycle_class = 'trade-product-gallery-nav slideshow vertical trade-opacity';
  }
  else {
    $cycle_class = 'trade-product-gallery-nav trade-opacity';
  }

  $vars['build']['images_gallery_table']['row']['cell_right']['gallery_container']['ul'] = array(
    '#markup' => '<div class="' . $cycle_class . '"
    data-cycle-fx=carousel
    data-cycle-timeout=0
    data-cycle-next="#' . $carousel_next . '"
    data-cycle-prev="#' . $carousel_prev . '"
    data-cycle-carousel-visible="' . $image_nav_view_count . '"
    data-cycle-carousel-vertical=true
    data-allow-wrap=false
    >' . implode("\n", $item_list) . '</div>'
      . '<div class="trade-gallery-carousel-nav-wr"><a class="trade-gallery-carousel-prev" href=# id=' . $carousel_prev . '><< Prev </a><a class="trade-gallery-carousel-next" href=# id=' . $carousel_next . '> Next >></a></div>',
  );
}

function theme_trade_product_image_gallery($vars) {
  return drupal_render($vars['build']);
}

function template_preprocess_trade_product_price_view(&$vars) {
  $currency = _trade_get_currency(variable_get('trade_currency', 'RUB'));
  $vars['build'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('trade-product-price'),
      'data-nid' => $vars['nid'],
    ),
    'amount_wr' => array(
      '#type' => 'html_tag',
      '#tag' => 'span',
      '#value' => _trade_format_number($vars['price']),
      '#attributes' => array(
        'class' => array('trade-product-price-amount'),
        'data-price' => $vars['price'],
        'data-price-view' => _trade_format_number($vars['price']),
      ),
    ),
    'currency_wr' => array(
      '#type' => 'html_tag',
      '#tag' => 'span',
      '#value' => $currency[1],
      '#attributes' => array('class' => array('trade-product-price-currency')),
    ),
  );
}

function theme_trade_product_price_view($vars) {
  return drupal_render($vars['build']);
}