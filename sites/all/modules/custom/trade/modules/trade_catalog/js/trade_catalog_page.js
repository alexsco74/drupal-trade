(function ($) {

    Drupal.behaviors.tradeProductFullPage = {
        attach: function (context, settings) {
            function tradeProductFullPageInit() {

                // related products masonry
                $('.trade-catalog-page-content').once().each(function () {
                    var thisContainer = $(this);
                    thisContainer.addClass('trade-masonry-process');
                    // set height on parent container
                    var countMasonryTradeItems = $('.trade-product-view-inner', thisContainer).length - 1;

                    thisContainer.ready(function () {
                        var countImage = $('img', thisContainer).length;
                        var numLoadImage = 0;
                        $('.trade-product-view img', thisContainer).once().load(function () {
                            numLoadImage = numLoadImage + 1;
                            if (countImage == numLoadImage) {
                                $('.trade-product-view-inside', thisContainer).each(function (a) {
                                    $(this).parent().css({'height': $(this).outerHeight() + 'px' });
                                    if (countMasonryTradeItems == a) {
                                        thisContainer.delay(100).masonry({
                                            columnWidth: 0,
                                            gutter: 0,
                                            transitionDuration: '0.1s',
                                            isResizable: false,
                                            itemSelector: '.trade-product-view'
                                        }).once(function () {
                                            thisContainer.addClass('trade-opacity-slide-show');
                                        });
                                    }
                                });
                            }
                        });
                    });
                });
            }

            tradeProductFullPageInit();

            $(document).bind('documentChange', function () {
                tradeProductFullPageInit();
            });
        }
    };

})(jQuery);