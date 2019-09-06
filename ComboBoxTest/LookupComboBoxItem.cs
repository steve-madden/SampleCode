using Prism.Mvvm;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ComboBoxTest
{
    public class LookupComboBoxItem : BindableBase
    {
        public LookupComboBoxItem(LookupItem item)
        {
            Item = item;
            _displayValue = Item.Name;
        }

        public LookupItem Item { get; }

        private string _displayValue = null;
        public string DisplayValue
        {
            get { return _displayValue; }
            set { SetProperty(ref _displayValue, value); }
        }
    }
}
