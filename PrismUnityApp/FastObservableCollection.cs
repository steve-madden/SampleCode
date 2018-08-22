using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace PrismUnityApp
{

    /// <summary>
    /// This is a specialized ObservableCollection that allows AddRange and Reset without 
    /// firing a flurry of CollectionChangedEvents.  It's useful for large collections bound to 
    /// WPF ListViews
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class FastObservableCollection<T> : ObservableCollection<T>
    {
        public FastObservableCollection() : base()
        {
        }

        public FastObservableCollection(IEnumerable<T> collection) : base(collection)
        {
        }

        public FastObservableCollection(List<T> list) : base(list)
        {
        }

        public void AddRange(IEnumerable<T> range)
        {
            foreach (var item in range)
            {
                Items.Add(item);
            }

            this.OnPropertyChanged(new PropertyChangedEventArgs("Count"));
            this.OnPropertyChanged(new PropertyChangedEventArgs("Item[]"));
            this.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        public void Reset(IEnumerable<T> range)
        {
            this.Items.Clear();
            AddRange(range);
        }

    }
}
