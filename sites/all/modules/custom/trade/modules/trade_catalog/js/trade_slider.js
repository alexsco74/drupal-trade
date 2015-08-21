(function ($) {

    Drupal.behaviors.tradeUiSlider = {
        attach: function (context, settings) {

            function tradeUiSliderInit() {


                $('.trade-numeric-slider-form-item').once().each(function () {
                    var thisPriceContainer = $(this);
                    var fieldName = $(this).attr('data-field');
                    var thisSlider = $('.trade-' + fieldName + '-slider-widget', thisPriceContainer);
                    var thisFromItem = $('.form-item-' + fieldName + '--from input:text', thisPriceContainer);
                    var thisToItem = $('.form-item-' + fieldName + '--to input:text', thisPriceContainer);
                    var thisDataMin = thisPriceContainer.attr('data-min');
                    var thisDataMax = thisPriceContainer.attr('data-max');
                    var thisDataEmin = parseInt(thisPriceContainer.attr('data-enable-min'), 10);
                    var thisDataEmax = parseInt(thisPriceContainer.attr('data-enable-max'), 10);
                    var thisFromItemVal = thisFromItem.val();
                    var thisToItemVal = thisToItem.val();

                    if (typeof thisDataEmin != undefined && typeof thisDataEmax != undefined && thisDataEmin == thisDataEmax) {
                        thisFromItem.attr('disabled', true);
                        thisToItem.attr('disabled', true);
                    }

                    var valueMin = thisFromItem.val() ? thisFromItem.val() : thisFromItem.attr('placeholder');
                    var valueMax = thisToItem.val() ? thisToItem.val() : thisToItem.attr('placeholder');

                    thisSlider.slider({
                        range: true,
                        min: thisDataEmin,
                        max: thisDataEmax,
                        values: [ valueMin, valueMax ],
                        slide: function (event, ui) {
                            $('.form-item-' + fieldName + '--from input:text', thisPriceContainer).val(ui.values[0]);
                            $('.form-item-' + fieldName + '--to input:text', thisPriceContainer).val(ui.values[1]);
                        }
                    });

                    // input min masked
                    thisFromItem.live('focus', function () {
                        thisFromItemVal = $(this).val();
                    });

                    thisFromItem.live('blur', function () {
                        var newFromItemVal = $(this).val();

                        if (newFromItemVal != parseInt(newFromItemVal, 10) || newFromItemVal < 1 || parseInt(newFromItemVal, 10) > parseInt($('.form-item-' + fieldName + '--to input:text', thisPriceContainer).val(), 10)) {
                            if (thisFromItemVal == parseInt(thisFromItemVal, 10) && thisFromItemVal > 0) {
                                $(this).val(thisFromItemVal);
                                return false;
                            } else {
                                $(this).val(thisDataEmin);
                            }
                        }
                    });

                    // input max masked
                    thisToItem.live('focus', function () {
                        thisToItemVal = $(this).val();
                    });

                    thisToItem.live('blur', function () {
                        var newToItemVal = $(this).val();

                        if (newToItemVal != parseInt(newToItemVal, 10) || newToItemVal < 1 || parseInt(newToItemVal, 10) < parseInt(thisFromItemVal, 10)) {
                            if (thisToItemVal == parseInt(thisToItemVal, 10) && thisToItemVal > 0) {
                                $(this).val(thisToItemVal);
                                return false;
                            } else {
                                $(this).val(thisDataEmax);
                            }
                        }
                    });

                    // change slider on change input
                    $('.form-item-' + fieldName + '--from input:text, .form-item-' + fieldName + '--to input:text', thisPriceContainer).live('change', function () {
                        if (parseInt($('.form-item-' + fieldName + '--from input:text', thisPriceContainer).val(), 10) <= parseInt($('.form-item-' + fieldName + '--to input:text', thisPriceContainer).val(), 10)) {
                            thisSlider.slider("values", [thisFromItem.val(), thisToItem.val()]);
                        }
                    });
                });
            };

            tradeUiSliderInit();

            $(document).bind('documentChange', function () {
                tradeUiSliderInit();
            });

        }
    };

})(jQuery);