(use-modules (rnrs))

;;; This function takes a three argument function as an argument and returns a curried version.
(define curry3
  (lambda (f)
    (lambda (x)
      (lambda (y)
	(lambda (z)
	  (f x y z))))))
;;; This function takes a curried three argument function as an argument and returns a non-curried version.
(define uncurry3
  (lambda (curryf)
    (lambda (x y z)
      (((curryf x) y) z))))

;;; This function takes a procedure and a list as arguments, and it returns a list of the elements of the given list for which procedure returns true.
(define my-filter
  (lambda (procedure lst)
    (if (null? lst) '()
	(if (procedure (car lst))
	   (cons (car lst) (my-filter procedure (cdr lst)))
	   (my-filter procedure (cdr lst))))))

;;; This function takes a procedure and a list as arguments, and it returns #f if at least one item in a list satisfies the procedure.
(define exists
  (lambda (procedure lst)
    (if (null? (my-filter procedure lst))
	#f
	#t)))

;;; This function takes a target word and a list (in a specific format) as arguments, and it returns the names of each document contained in the list that matches the targeted word.
(define find-docs
  (lambda (target doclist)
    (let ((contains (find-docs-helper target)))
    (filter not-unspecified (map contains doclist)))))

;;; This curried helper function takes a target word as an argument and returns a function which takes a list as a function which returns a name of the document if the document contains the target word. Else return unspecified.
(define find-docs-helper
  (lambda (target)
    (lambda (doclist)
      (if (member target (car (cdr doclist)))
	  (car doclist)))))

;;; This function returns true if its argument is not unspecified, and false if it is.
(define not-unspecified
  (lambda (x)
    (not (unspecified? x))))

;;; This function acts similarly to map, but flattens the result into one list
(define flatmap
  (lambda (procedure lst)
    (if (null? lst)
	'()
	(let ((result (procedure (car lst))))
	  (if (list? result)
	      (append result (flatmap procedure (cdr lst)))
	      (append (list result) (flatmap procedure (cdr lst))))))))
    
;;;
(define contains
  (lambda (elem1)
    (lambda (elem2)
      (if (equal? elem1 elem2)
	  #t
	  #f))))

;;;

;;;
(define intersect
  (lambda (set1 set2)
    (if (null? set1)
	'()
	(append (filter (contains (car set1)) set2)
		(intersect (cdr set1) set2)))))
