<div<?php print $attributes; ?>>
  <header class="l-header" role="banner">

    <div class="l-header-top">
      <div class="l-header-top-line">

        <div class="l-branding l-header-top-col l-header-top-left">
          <?php if ($logo): ?>
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="site-logo"><img
                src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>"/></a>
          <?php endif; ?>

          <?php if ($site_name): ?>
            <h1 class="site-name">
              <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>"
                 rel="home"><span><?php print $site_name; ?></span></a>
            </h1>
          <?php endif; ?>

          <?php print render($page['branding']); ?>
        </div>

        <div class="l-header-top-col l-header-top-right">
          <div class="l-region l-region--header-wrapper">
            <?php print render($page['header']); ?>
          </div>
        </div>
      </div>
    </div>

    <div class="l-region l-region--slogan">
      <?php if ($site_slogan): ?>
        <h2 class="site-slogan"><?php print $site_slogan; ?></h2>
      <?php endif; ?>
    </div>



    <?php print render($page['navigation']); ?>
  </header>

  <div class="l-main">
    <div class="l-content" role="main">
      <?php print render($page['highlighted']); ?>
      <?php print $breadcrumb; ?>
      <a id="main-content"></a>
      <?php print render($title_prefix); ?>
      <?php if ($title): ?>
        <h1 class="page-title"><?php print $title; ?></h1>
      <?php endif; ?>
      <?php print render($title_suffix); ?>
      <?php print $messages; ?>
      <?php print render($tabs); ?>
      <?php print render($page['help']); ?>
      <?php if ($action_links): ?>
        <ul class="action-links"><?php print render($action_links); ?></ul>
      <?php endif; ?>
      <?php print render($page['content']); ?>
      <?php print $feed_icons; ?>
    </div>

    <?php print render($page['sidebar_first']); ?>
    <?php print render($page['sidebar_second']); ?>
  </div>

  <footer class="l-footer" role="contentinfo">
    <?php print render($page['footer']); ?>
    <?php if ($page['footer_first'] || $page['footer_second']): ?>
      <div class="l-table">
        <div class="l-table-row">
          <div class="l-table-cell l-left-side">
            <?php print render($page['footer_first']); ?>
          </div>
          <div class="l-table-cell l-center-side"></div>
          <div class="l-table-cell l-right-side">
            <?php print render($page['footer_second']); ?>
          </div>
        </div>
      </div>
    <?php endif; ?>
  </footer>
</div>
