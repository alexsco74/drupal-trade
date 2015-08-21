(function ($) {

    Drupal.behaviors.customBfeFormItemCheckbox = {
        attach: function (context, settings) {

            function customBfeFormItemCheckboxInit() {
                $('input:checkbox', context).each(function () {
                    var thisEl = $(this);
                    thisEl.parent().addClass('custom-bef-checkbox');
                });
            };

            customBfeFormItemCheckboxInit();

            $(document).bind('documentChange', function () {
                customBfeFormItemCheckboxInit();
            });

        }
    };

})(jQuery);