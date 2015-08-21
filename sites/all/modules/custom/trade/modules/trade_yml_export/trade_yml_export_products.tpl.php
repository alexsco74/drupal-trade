<?php
echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" ?>
<!DOCTYPE yml_catalog SYSTEM "shops.dtd">
<yml_catalog date="<?php echo date('Y-m-d h:i'); ?>">
<shop>
<name><?php echo variable_get('site_name', 'Drupal'); ?></name>
  <company><?php echo variable_get('site_name', 'Drupal'); ?></company>
  <url><?php echo $GLOBALS['base_url']; ?></url>

<currencies><currency id="RUR" rate="1"/></currencies>
<categories>
	<?php foreach ($categories as $term): ?>
	<category id="<?php echo $term->tid ?>"<?php if($term->parent): ?> parentId="<?php echo $term->parent ?>"<?php endif; ?>><?php echo trade_yml_export_ys($term->name)?></category>
	<?php endforeach; ?>
</categories>

<offers>
  <?php
  foreach ($nodes as $node):
    $descr = field_get_items('node', $node, $desc_field);
    if (isset($descr[0])) {
      $descr = $descr[0];
    }

    $image = field_get_items('node', $node, $image_field);
    if (isset($image[0])) {
      $image = file_create_url($image[0]['uri']);
    }
    $price = !empty($node->trade_product['price']['trade_product_price']) ? $node->trade_product['price']['trade_product_price'] : '1'; //must be not null
?>
	<offer id="<?php echo $node->nid; ?>" available="true">
	<url><?php echo url('node/' . $node->nid, array('absolute' => TRUE)); ?></url>
	<price><?php echo $price; ?></price>
	<currencyId><?php echo $currency_code; ?></currencyId>
	<categoryId><?php echo $nodes_tid[$node->nid]; ?></categoryId>
	<?php if (!empty($image)): ?>
	<picture><?php echo $image; ?></picture>
	<?php endif; ?>
	<delivery><?php echo variable_get('yml_export_delivery', 'true'); ?></delivery>
	<name><?php echo trade_yml_export_ys(check_plain($node->title)) ?></name>
	<description><?php echo trade_yml_export_ys(truncate_utf8(strip_tags($descr['value']), 255, TRUE)) ?></description>
	<sales_notes></sales_notes>
	</offer>
	<?php
    endforeach;
    ?>
</offers>

</shop>
</yml_catalog>