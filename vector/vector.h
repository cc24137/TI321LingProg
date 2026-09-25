#ifndef VECTOR
#define VECTOR

typedef void* Elemento;

typedef enum {
    false,
    true
} boolean;

// link do vector.java https://docs.oracle.com/javase/8/docs/api/java/util/Vector.html
class Vector 
{
    private:
        Elemento lista[100];
        unsigned int tamanhoLogico;
        unsigned int tamanhoFisico;
        unsigned int incremento;
    public:

        // ver uso de collection

        Vector(); // construtor
        Vector(Vector());
        Vector(unsigned int);
        Vector(unsigned int, unsigned int)
        ~Vector(); // destrutor
        Elemento& operator=(const Vector&); // operator copia
        Vector(const Vector&); // construtor copia
    
    
        Elemento& operator[](unsigned int);

        boolean add(Elemento);
        void add(unsigned int, Elemento);
        boolean addAll(const Vector()&);
        boolean addAll(unsigned int, const Vector()&);
        void addElement(Elemento); //???? nao sei oq é isso
        unsigned int capacity();
        void clear();
        Vector clone(); // ver retorno
        boolean contains(const Elemento&); // recebe Object?
        boolean containsAll(Elemento[]) // collection??
        void copyInto(Elemento[]); // Object[]
        const Elemento& objectAt(unsigned int);
        const Elemento[] elements(); // ver oq retorna
        void ensureCapacity(unsigned int);
        bolean equals(Vector)
        Elemento& firstElement();
        void forEach(); //???????????????????
        int hashCode();
        int indexOf(const Elemento&);
        int indexOf(const Elemento&, unsigned int);
        void insertElementAt(Elemento, unsigned int);
        boolean isEmpty();
        // Iterator<Vector> iterator(); // ?????
        Elemento& lastElement();
        int lastIndexOf(const Elemento&);
        int lastIndexOf(const Elemento&, unsigned int);
        // ListIterator<E> listIterator();
        // ListIterator<E> listIterator(unsigned int);
        const Elemento& remove(unsigned int);
        boolean Elemento& remove(const Elemento&);
        boolean removeAll(Vector);
        void removeAllElements();
        boolean removeElement(const Elemento&);
        void removeElementAt(unsigned int);
        //boolean	removeIf(Predicate<? super E> filter)	Removes all of the elements of this collection that satisfy the given predicate.
        protected void removeRange(unsigned int, unsigned int);
        void replaceAll(UnaryOperator<Elemento> operator);
        boolean retainAll(Vector);
        void setSize(unsigned int);
        unsigned int size();
        void sort(); // recebe um Comparator
        //Spliterator<E>	spliterator()	Creates a late-binding and fail-fast Spliterator over the elements in this list.
        Elemento[] subList(unsigned int, unsigned int); // ver oq retorna
        //<T> T[]	toArray(T[] a)	        Returns an array containing all of the elements in this Vector in the correct order; the runtime type of the returned array is that of the specified array.
        String toString();
        void trimToSize();
}

#endif

