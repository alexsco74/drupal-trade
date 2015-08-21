(function ($) {

    Drupal.behaviors.tradeProductFullPage = {
        attach: function (context, settings) {


            // upholstery load
            var waitTag = $('<div class="trade-ajax-load-overlay"></div><span class="trade-ajax-load-process-wait"></span>');

            function tradeProductFullPageLoadUpholsteryPicker() {
                $('.trade-product-option-upholstery-picker-container-ajax-load').each(function () {

                    var thisContainer = $(this);

                    var dataNid = $(this).attr('data-nid');
                    var link = '/trade/product/' + dataNid + '/upholstery';
                    // thisContainer.addClass('trade-ajax-load-process').append(waitTag);
                    var cloneWait = waitTag.clone();
                    thisContainer.addClass('trade-ajax-load-process');
                    thisContainer.prepend(cloneWait);
                    $.ajax({
                        type: 'POST',
                        data: {
                            trade_nid: dataNid
                        },
                        url: link,
                        success: function (response) {
                            if (response.status) {
                                if (response.data) {
                                    thisContainer.replaceWith(response.data);
                                    $(document).trigger('documentChange');
                                    // $('.trade-order-container').fadeIn();
                                }
                                cloneWait.remove();
                                thisContainer.removeClass('trade-ajax-load-process');
                            }

                            // open all
                            cloneWait.remove();
                            thisContainer.removeClass('trade-ajax-load-process');
                            // $('.trade-ajax-load-process-wait', thisContainer).remove();
                        },
                        error: function (xmlhttp) {
                            cloneWait.remove();
                            thisContainer.removeClass('trade-ajax-load-process');
                            // $('.trade-ajax-load-process-wait', thisContainer).remove();
                            // alert(Drupal.ajaxError(xmlhttp, link));
                        }
                    });

                });
            };

            $(window).ready(function () {
                tradeProductFullPageLoadUpholsteryPicker();
            });

            // count pane
            $('.panels-flexible-region-inside-last .pane-node-trade-product-price:first').once().each(function () {
                var thisPrice = $(this);
                var countContainer = $('.panel-pane', thisPrice.parent()).length;

                if (countContainer > 2) {
                    thisPrice.addClass('trade-separator-with');
                }
            });

            // related products masonry
            $('.trade-product-teaser-list').once().each(function () {
                var thisContainer = $(this);
                var countMasonryTradeItems = $('.trade-product-view-inner', thisContainer).length - 1;
                thisContainer.ready(function () {
                    var countImage = $('img', thisContainer).length;
                    var numLoadImage = 0;
                    $('.trade-product-view img', thisContainer).once().load(function () {
                        numLoadImage = numLoadImage + 1;
                        if (numLoadImage == countImage) {
                            $('.trade-product-view-inside', thisContainer).each(function (a) {
                                $(this).parent().css({'height': $(this).outerHeight() + 'px' });
                                if (countMasonryTradeItems == a) {
                                    thisContainer.delay(1000).masonry({
                                        columnWidth: 0,
                                        gutter: 0,
                                        transitionDuration: '0.1s',
                                        isResizable: false,
                                        itemSelector: '.trade-product-view'
                                    });
                                }
                            });
                        }
                    });
                }).once(function () {
                    thisContainer.addClass('trade-opacity-slide-show');
                });
            });
        }
    };

})(jQuery);