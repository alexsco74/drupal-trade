(function ($) {

    Drupal.behaviors.tradeAddToOrder = {
        attach: function (context, settings) {

            var currencySeparator = Drupal.settings.trade.currencySeparator;

            function tradeAddToOrderInit() {
                $('.trade-product-add-to-order').each(
                    function () {
                        var thisForm = $(this);
                        var thisNid = thisForm.attr('data-nid');

                        function tradeAddToOrderCheckSetPrice() {
                            var thisEl = $(this);
                            var priceEl = $('.trade-product-price[data-nid="' + thisNid + '"] .trade-product-price-amount');
                            var basePrice = parseInt(priceEl.attr('data-price'), 10);
                            var basePriceView = priceEl.attr('data-price-view');
                            var priceUpholstery = 0;
                            var pricePlusSum = 0;
                            var selPrice = 0;

                            // check & set price
                            $('.trade-product-option-form-item', thisForm).each(
                                function () {
                                    var itemPriceType = $(this).attr('data-ptype');
                                    var selPrice = $('option:selected', $(this)).attr('data-price');
                                    if (selPrice == undefined) {
                                        selPrice = 0;
                                    } else {
                                        selPrice = parseInt(selPrice, 10);
                                    }
                                    var selPriceView = $('option:selected', $(this)).attr('data-price-view');
                                    if (itemPriceType != undefined && itemPriceType == 'upholstery') {

                                        // upholstery change base price
                                        if (selPrice > basePrice) {
                                            basePrice = selPrice;
                                            basePriceView = selPriceView;
                                        }
                                    } else {

                                        // add price
                                        pricePlusSum = pricePlusSum + selPrice;
                                    }
                                }
                            )

                            basePrice = basePrice + pricePlusSum;
                            basePriceView = basePrice.toString().replace(/(\d)(?=(\d{3})+(?!\d))/g, "$1" + currencySeparator);
                            priceEl.html(basePriceView);
                        };

                        //option default
                        tradeAddToOrderCheckSetPrice();

                        //back set picker from option for validate form
                        $('select[data-nid][data-field][data-relation="slave"]', thisForm).each(function () {
                            if (!$(this).hasClass('trade-product-add-to-order-option-process') && $(this).val() != '_none') {
                                var dataField = $(this).attr('data-field');
                                var dataNid = $(this).attr('data-nid');
                                $('input[value="' + $(this).val() + '"][data-nid="' + dataNid + '"][data-field="' + dataField + '"][data-relation="master"]:not(:checked)')
                                    .trigger('click');
                                $(this).addClass('trade-product-add-to-order-option-process')
                            }
                        });

                        $('.trade-product-option-form-item', thisForm).bind('change', function () {
                            tradeAddToOrderCheckSetPrice();
                        });

                        // relation with add to order
                        $('.form-radios[data-relation="master"]').once().each(function () {
                            var thisRadiosWr = $(this);
                            var dataField = thisRadiosWr.attr('data-field');
                            var dataNid = thisRadiosWr.attr('data-nid');

                            // hide slave
                            if (Drupal.settings.trade.debugMode == undefined) {
                                $('[data-relation="slave"][data-nid="' + dataNid + '"][data-field="' + dataField + '"]').parent().hide();
                            }

                            // event change
                            $('input:radio', thisRadiosWr).bind('change', function () {
                                if (typeof $('[data-relation="slave"][data-nid="' + dataNid + '"][data-field="' + dataField + '"]') != undefined) {
                                    var newVal = $(this).val();
                                    $('[data-relation="slave"][data-nid="' + dataNid + '"][data-field="' + dataField + '"]').each(function (n) {
                                            if (n == 0) {
                                                $(this).val(newVal).trigger('change');
                                            }
                                        }
                                    );
                                }
                            });
                        });
                    }
                );
            };

            tradeAddToOrderInit();

            $(document).bind('documentChange', function () {
                tradeAddToOrderInit();
            });

        }
    };

})(jQuery);
