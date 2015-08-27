(function ($) {

    Drupal.behaviors.tradeCatalogFilter = {
        attach: function (context, settings) {

            function tradeCatalogFilterInit() {
                $('.trade-catalog-filters input:checkbox[title]', context).each(function () {
                    var thisEl = $(this);
                    $('label', thisEl.parent()).attr('title', thisEl.attr('title'));
                });
            };

            tradeCatalogFilterInit();

            $(document).bind('documentChange', function () {
                tradeCatalogFilterInit();
            });

        }
    };

})(jQuery);