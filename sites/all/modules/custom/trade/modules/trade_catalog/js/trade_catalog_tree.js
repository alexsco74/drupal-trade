(function ($) {

    Drupal.behaviors.tradeCatalogTree = {
        attach: function (context, settings) {
            var openLink = $('<span class="trade-catalog-link-tree-manager">' + Drupal.settings.trade.open + '</span>');


            $('.block--menu-menu-catalog').once().each(function () {
                var thisContainer = $(this);
                $('ul.menu li.expanded', thisContainer).each(function () {
                    var thisLi = $(this);
                    if ($('a.active', thisLi).length == 0) {
                        var thisOpenLink = openLink.clone();
                        $('a:first', thisLi).after(thisOpenLink);
                        thisOpenLink.click(function () {
                            if ($(this).hasClass('trade-is-open')) {
                                $(this).removeClass('trade-is-open');
                                $(this).text(Drupal.settings.trade.open);
                            } else {
                                $(this).addClass('trade-is-open');
                                $(this).text(Drupal.settings.trade.close);
                            }
                            $('ul:first', thisLi).slideToggle();
                        });
                        $('ul', thisLi).hide();
                    }
                });
            });
        }
    };

})(jQuery);