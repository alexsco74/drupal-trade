(function ($) {

    Drupal.behaviors.tradeOption = {
        attach: function (context, settings) {

            // relation price with use
            $('input.trade-option-price').once().each(function () {
                var thisElement = $(this);
                thisElement.live('change', function () {
                    if ($(this).val() == '' && $('input.trade-option-use:checked', thisElement.parent().parent().parent()).length
                        || $(this).val() != '' && !$('input.trade-option-use:checked', thisElement.parent().parent().parent()).length) {
                        $('input.trade-option-use', thisElement.parent().parent().parent()).trigger('click');
                    }

                    //option default reset
                    if ($(this).val() == '' && $('input.trade-option-is-default:checked', thisElement.parent().parent().parent()).length) {
                        $('input.trade-option-is-default', thisElement.parent().parent().parent()).trigger('click');
                    }

                });
            });

            // relation use with default
            $('input.trade-option-use').once().each(function () {
                var thisElement = $(this);
                thisElement.live('change', function () {

                    //option default reset
                    if (!thisElement.is(':checked')) {
                        $('input.trade-option-is-default:checked', thisElement.parent().parent().parent()).trigger('click');
                    }
                });
            });

            //option default
            $('input.trade-option-is-default').once().each(function () {
                var thisElement = $(this);
                thisElement.live('change', function () {
                    if (thisElement.is(':checked')) {
                        var curId = thisElement.attr('id');
                        $('input.trade-option-is-default:checked:not(#' + curId + ')', thisElement.parent().parent().parent().parent()).trigger('click');
                    }
                });
            });
        }
    };

})(jQuery);