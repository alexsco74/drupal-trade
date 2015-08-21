(function ($) {

    Drupal.behaviors.tradeSpinner = {
        attach: function (context, settings) {

            // Add a custom spinner on quantity widget.
            function tradeSpinner() {
                $('input.trade-spinner').once().each(function () {
                    var thisInput = $(this);
                    var thisInputParent = thisInput.parent();
                    var thisInputMaxLength = thisInput.attr('maxlength');

                    var currVal = thisInput.val();
                    thisInputParent.addClass('trade-spinner-container');

                    // input masked
                    thisInput.bind('focus', function () {
                        currVal = $(this).val();
                    });

                    thisInput.bind('change', function () {
                        var newVal = $(this).val();

                        if (newVal != parseInt(newVal, 10) || newVal < 1) {
                            if (currVal == parseInt(currVal, 10) && currVal > 0) {
                                $(this).val(currVal);
                                return false;
                            } else {
                                $(this).val(1);
                            }
                        }

                    });

                    //minus
                    var minus = $('<span class="trade-spinner-button-wr"><a href="#" class="trade-spinner-button trade-spinner-minus">Minus</a></span>').click(function (e) {
                        e.preventDefault();
                        if (!$(this).hasClass('trade-disabled') && thisInput.val() > 1) {
                            thisInput.val(parseInt(thisInput.val(), 10) - 1).trigger('change');
                        }
                        return false;
                    });
                    thisInput.before(minus);

                    // plus
                    var plus = $('<span class="trade-spinner-button-wr"><a href="#" class="trade-spinner-button trade-spinner-plus">Plus</a></span>').click(function (e) {
                        e.preventDefault();
                        var validLength = true;

                        var nextValue = (parseInt(thisInput.val(), 10) + 1) + '';

                        if(typeof thisInputMaxLength != undefined && nextValue.length > thisInputMaxLength){
                            validLength = false;
                        }

                        if (!$(this).hasClass('trade-disabled') && validLength) {
                            thisInput.val(parseInt(thisInput.val(), 10) + 1).trigger('change');
                        }
                        return false;
                    });
                    thisInput.after(plus);
                });
            };

            tradeSpinner();

            $(document).bind('documentChange', function () {
                tradeSpinner();
            });
        }
    };
})(jQuery);