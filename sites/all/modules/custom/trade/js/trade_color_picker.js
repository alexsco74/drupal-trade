(function ($) {

    Drupal.behaviors.tradeColorPicker = {
        attach: function (context, settings) {

            function tradeColorPickerInit() {
                // relation price with use
                $('.trade-product-option-color-picker').once().each(function () {
                    var thisEl = $(this);

                    // tooltip
                    thisEl.tooltip({
                        items: ".trade-product-color-picker-option-thumb",
                        tooltipClass: 'trade-product-color-picker',
                        content: function () {
                            var thisWr = $(this).parent();
                            if (typeof $('.trade-tooltip-content', thisWr) != undefined) {
                                return $('.trade-tooltip-content', thisWr).html();
                            }
                        }
                    });

                });
            };

            tradeColorPickerInit();

            $(document).bind('documentChange', function () {
                tradeColorPickerInit();
            });

        }
    };

})(jQuery);