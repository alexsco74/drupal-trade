(function ($) {

    Drupal.behaviors.tradeCart = {
        attach: function (context, settings) {

            $('.trade-cart-block-inner').once().each(function () {
                $(this).addClass('trade-cart-ajax-load');
                $('a', $(this)).append('<span class="trade-process">');
            });

            $(window).load(function () {
                $('.trade-cart-block-inner.trade-cart-ajax-load:first').load('/trade/cart .trade-cart-block-inner > *', function (response, status, xhr) {
                    if (status != 'error') {
                        $('.trade-cart-block-inner.trade-cart-ajax-load:first').removeClass('trade-cart-ajax-load');
                    }
                });
            });
        }
    };

})(jQuery);