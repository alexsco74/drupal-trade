(function ($) {

    Drupal.behaviors.tradeProductOptionPicker = {
        attach: function (context, settings) {

            function tradeProductOptionPickerInit() {
                var overlay = $('<div class="trade-product-option-vertical-overlay" style="display:none;"></div>');

                // relation price with use
                $('.trade-product-option-picker').once().each(function () {
                    var thisEl = $(this);

                    // tooltip
                    thisEl.tooltip({
                        items: ".trade-product-picker-option-inner",
                        tooltipClass: 'trade-product-option-picker',
                        content: function () {
                            var thisWr = $(this).parent();
                            if (typeof $('.trade-tooltip-content', thisWr) != undefined) {
                                return $('.trade-tooltip-content', thisWr).html();
                            }
                        }
                    });

                });

            };

            tradeProductOptionPickerInit();

            $(document).bind('documentChange', function () {
                tradeProductOptionPickerInit();
            });

        }
    };

})(jQuery);