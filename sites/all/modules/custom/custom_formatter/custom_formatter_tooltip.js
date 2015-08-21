(function ($) {

    Drupal.behaviors.customFormatterTooltip = {
        attach: function (context, settings) {
            $('.custom-formatter-tooltip-wr').once().each(function () {
                var thisEl = $(this);
                $('.custom-formatter-tooltip-title').click(function (e) {
                    e.preventDefault();
                    $(this).trigger('mouseover');
                    return false;
                });
                // tooltip
                thisEl.tooltip({
                    items: ".custom-formatter-tooltip-title",
                    tooltipClass: 'custom-formatter-tooltip',
                    content: function () {
                        var thisWr = $(this).parent();
                        if (typeof $('.custom-formatter-tooltip-content', thisWr) != undefined) {
                            return $('.custom-formatter-tooltip-content', thisWr).html();
                        }
                    }
                });
            });
        }
    };
})(jQuery);