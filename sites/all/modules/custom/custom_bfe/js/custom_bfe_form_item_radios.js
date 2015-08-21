(function ($) {

    Drupal.behaviors.customBfeFormItemRadios = {
        attach: function (context, settings) {

            function customBfeFormItemRadiosInit() {
                $('input:radio', context).each(function () {
                    var thisEl = $(this);
                    $('input:radio').parent().addClass('custom-bef-radio');
                });
            }

            // customBfeFormItemRadiosInit();

            $(document).bind('documentChange', function () {
                customBfeFormItemRadiosInit();
            });
        }
    };

})(jQuery);