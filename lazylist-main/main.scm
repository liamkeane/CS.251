;;; gen-list will generate a list of consecutive integers from start to stop
(define gen-list
  (lambda (start stop)
    (if (> start stop)
	'()
	(cons start (gen-list (+ 1 start) stop)))))

;;; This predicate tests whether any two adjacent vales in lst sum to val
(define pair-sum?
  (lambda (lst val)
    (cond ((null? lst) #f)
	  ((null? (cdr lst)) #f)
	  ((equal? val (+ (car lst)
			  (car (cdr lst)))) #t)
	  (else (pair-sum? (cdr lst) val)))))

;;; This predicate tests whether any two adjacent values in a lazy list sum to val
(define pair-sum-lazy?
  (lambda (lst val)
    (cond ((null? lst) #f)
	  ((equal? #f ((cdr lst))) #f)
	  ((equal? val (+ (car lst)
			  (car ((cdr lst))))) #t)
	  (else (pair-sum-lazy? ((cdr lst)) val)))))

;;; example function that defines a lazy list of consecutive integers
(define gen-lazy-list
  (lambda (start stop)
    (if (> start stop)
        #f
        (cons start
              (lambda () (gen-lazy-list (+ start 1) stop))))))

;;; This function takes an integer and returns an integer lazy list containing the indefinite sequence of values.
(define inf-seq
  (lambda (first)
    (cons first
	  (lambda () (inf-seq (+ first 1))))))

;;; This function takes a lazy list and n as a parameter and returns the first n items in the list
(define first-n
  (lambda (lazy-list n)
    (cond ((null? lazy-list) '())
	  ((equal? lazy-list #f) '())
	  ((equal? 0 n) '())
	  (else (cons (car lazy-list)
		      (first-n ((cdr lazy-list))
			       (- n 1)))))))

;;; This function takes a lazy list and an integer and returns the n-th value in the lazy-list (counting from 1). If the lazy-list contains fewer than n values, then #f is returned.
(define nth
  (lambda (lazy-list n)
    (cond ((null? lazy-list) #f)
	  ((equal? lazy-list #f) #f)
	  ((equal? n 1) (car lazy-list))
	  (else (nth ((cdr lazy-list))
		     (- n 1))))))

;;; This helper function calculates if x is a multiple of n.
;;; Found modulo function: https://stackoverflow.com/questions/60801831/how-modulo-and-remainder-should-work-in-scheme
(define multiple?
  (lambda (x n)
    (if (equal? (modulo x n)
		0)
	#t
	#f)))


;;; This function returns a new lazy list that has n and all integer multiples of n removed from lazy-list
(define filter-multiples
  (lambda (lazy-list n)
    (cond ((equal? lazy-list #f) #f)
	  ((multiple? (car lazy-list) n)
	   (filter-multiples ((cdr lazy-list)) n))
	  (else (cons (car lazy-list)
		      (lambda () (filter-multiples ((cdr lazy-list)) n)))))))

;;; make-lazy returns a lazy version of an inputed scheme list.
(define make-lazy
  (lambda (lst)
    (if (null? lst)
	#f
	(cons (car lst) (lambda () (make-lazy (cdr lst)))))))
