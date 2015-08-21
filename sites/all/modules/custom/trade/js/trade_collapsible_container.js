(function ($) {

    Drupal.behaviors.tradeCollapsibeContainer = {
        attach: function (context, settings) {
            $('.trade-collapsible-container').once().each(function () {
                var thisCont = $(this);

                //init
                if (thisCont.hasClass('trade-collapsible-container-close')) {
                    $('.trade-collapsible-container-content', thisCont).hide();
                }

                //event
                $('.trade-collapsible-container-link', thisCont).on('click', function () {
                    if (thisCont.hasClass('trade-collapsible-container-close')) {
                        thisCont.removeClass('trade-collapsible-container-close');
                    } else {
                        thisCont.addClass('trade-collapsible-container-close');
                    }
                    $('.trade-collapsible-container-content', thisCont).toggle();

                });
            });
        }
    };

})(jQuery);